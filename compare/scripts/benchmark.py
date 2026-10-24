#!/usr/bin/env python3
"""Measure serial / OpenMP / MPI / hybrid wall-clock time across worker counts and
write compare/data/comparison.{serial,omp,mpi,hybrid}.csv in the project's schema.

The four variants are the project's four deliverables. "mpi" is the pure MPI solver
scaled by process count; "hybrid" is the MPI+OpenMP solver, measured with threads
actually on (HYBRID_THREADS per rank) because measuring it at one thread would only
reproduce the mpi column.

Every timed run is also checked for correctness: its output is compared against
the instance's reference .out file and the number of differing recommendations
goes in the `resultsAreCorrect` column (0 = no mismatches, as the old
verifyResult module reported it). A mismatch is surfaced on stderr and makes the
script exit non-zero, after the data is written -- an incorrect run must never be
published as a clean measurement.

The whole end-to-end run time is recorded in the `loop` phase column (the
dominant phase for these compute-bound instances) so that graphs.R, which derives
total = readInput + initialLR + loop + finalFiltering, reproduces it. The other
phase columns are written as 0 because nothing measures them: in a distributed run
a phase time is per-rank and needs a barrier to aggregate, which would perturb the
very thing being timed.

The configuration each dataset was produced under -- machine, core count, worker
counts, repetitions and how they reduce to one figure -- is written next to the
CSVs as benchmark-config.json, so published data always carries its provenance.

Environment overrides (all optional):
  SERIAL_BIN, OMP_BIN, MPI_BIN,
  HYBRID_BIN                      paths to the four executables
  HYBRID_THREADS                  threads per rank for the hybrid variant (default 2)
  MPIRUN                          mpirun/mpiexec launcher (default: mpirun)
  INSTANCES_DIR, OUT_DIR          input instances / output csv directory
  INSTANCES                       space-separated instance names (no extension)
  PARALLELISM                     space-separated worker counts (capped to #cores)
  REPS                            repetitions per measurement; the minimum is kept
"""
import csv, datetime, json, os, platform, subprocess, time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


def env(key, default):
    return os.environ.get(key, default)


SERIAL = env("SERIAL_BIN", "/tmp/mf_serial")
OMP = env("OMP_BIN", "/tmp/mf_omp")
MPI = env("MPI_BIN", "/tmp/mf_mpi")
HYBRID = env("HYBRID_BIN", "/tmp/mf_mpi_hybrid")
MPIRUN = env("MPIRUN", "mpirun")
INSTDIR = env("INSTANCES_DIR", os.path.join(ROOT, "instances"))
OUTDIR = env("OUT_DIR", os.path.join(ROOT, "compare", "data"))
REPS = int(env("REPS", "3"))
HYBRID_THREADS = int(env("HYBRID_THREADS", "2"))
NCPU = os.cpu_count() or 1

# Cap parallelism at the core count: oversubscribing corrupts timing.
PARALLELISM = [int(x) for x in env("PARALLELISM", "1 2 4").split()]
PARALLELISM = [p for p in PARALLELISM if p <= NCPU] or [1]

INSTANCES = env(
    "INSTANCES",
    "inst1000-1000-100-2-30 inst500-500-20-2-100 inst200-10000-50-100-300",
).split()

HEADER = ["fileName", "numberOfThreads", "resultsAreCorrect", "numberOfUsers",
          "numberOfItems", "numberOfFeatures", "numberOfNonZeroElements",
          "numberOfIterations", "readInput", "initialLR", "loop",
          "finalFiltering", "total"]


def dims(inst):
    with open(os.path.join(INSTDIR, inst + ".in")) as f:
        head = [next(f).strip() for _ in range(4)]
    users, items, nnz = (int(x) for x in head[3].split()[:3])
    return dict(iters=int(head[0]), feats=int(head[2]),
                users=users, items=items, nnz=nnz)


def expected(inst):
    """The instance's reference recommendations, one per user."""
    with open(os.path.join(INSTDIR, inst + ".out")) as f:
        return f.read().split()


def mismatches(output, reference):
    """How many recommendations differ from the reference.

    One mismatch per user whose recommended item differs, plus any missing or
    extra lines, mirroring what the old verifyResult module counted.
    """
    got = output.split()
    common = min(len(got), len(reference))
    differing = sum(1 for i in range(common) if got[i] != reference[i])
    return differing + abs(len(got) - len(reference))


def best(cmd, reference, extra_env=None):
    """Fastest of REPS runs, with the worst correctness result seen across them.

    Output is captured rather than discarded so it can be checked; the pipe is
    part of the measured interval for every variant equally, so comparisons
    between them stay valid.
    """
    e = dict(os.environ)
    if extra_env:
        e.update(extra_env)
    times, worst = [], 0
    for _ in range(REPS):
        start = time.perf_counter()
        r = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                           env=e, universal_newlines=True)
        elapsed = time.perf_counter() - start
        if r.returncode != 0:
            raise SystemExit("command failed (%d): %s" % (r.returncode, " ".join(cmd)))
        worst = max(worst, mismatches(r.stdout, reference))
        times.append(elapsed)
    return min(times), worst


def describe(cmd):
    """First line of a tool's version output, or None if it cannot be run."""
    try:
        r = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                           universal_newlines=True)
        return r.stdout.strip().split("\n")[0] or None
    except OSError:
        return None


def write_config(bad_total):
    """Record what produced this dataset, next to the dataset itself."""
    cfg = {
        "measuredAt": datetime.datetime.now(datetime.timezone.utc)
                              .strftime("%Y-%m-%dT%H:%M:%SZ"),
        "machine": {
            "platform": platform.platform(),
            "processor": platform.processor() or platform.machine(),
            "cores": NCPU,
        },
        "instances": INSTANCES,
        "workerCounts": PARALLELISM,
        "hybridThreadsPerRank": HYBRID_THREADS,
        "repetitions": REPS,
        "reduction": "minimum of %d repetitions" % REPS,
        "timing": "end-to-end wall clock, recorded in the loop column; "
                  "other phase columns are not measured",
        "correctness": "every timed run compared against the instance .out file; "
                       "resultsAreCorrect is the number of differing recommendations",
        "variants": {
            "serial": "sequential baseline",
            "omp": "OpenMP, numberOfThreads is threads",
            "mpi": "pure MPI, numberOfThreads is processes",
            "hybrid": "MPI+OpenMP, numberOfThreads is processes at "
                      "%d thread(s) per rank" % HYBRID_THREADS,
        },
        "totalMismatches": bad_total,
        "mpirun": describe([MPIRUN, "--version"]),
        "commit": describe(["git", "-C", ROOT, "rev-parse", "--short", "HEAD"]),
    }
    path = os.path.join(OUTDIR, "benchmark-config.json")
    with open(path, "w") as f:
        json.dump(cfg, f, indent=2)
        f.write("\n")
    return path


def main():
    rows = {"serial": [], "omp": [], "mpi": [], "hybrid": []}
    nonlocal_counts = [0, 0, 0]   # differing recommendations, bad runs, runs
    for inst in INSTANCES:
        d = dims(inst)
        infile = os.path.join(INSTDIR, inst + ".in")
        name = "../instances/%s.in" % inst
        reference = expected(inst)

        def row(threads, measured, label):
            sec, bad = measured
            if bad:
                print("  !! %s %s: %d recommendation(s) differ from %s.out"
                      % (inst, label, bad, inst), flush=True)
            return (bad, [name, threads, bad, d["users"], d["items"], d["feats"],
                          d["nnz"], d["iters"], 0, 0, round(sec, 4), 0, round(sec, 4)])

        def record(ver, threads, measured, label):
            bad, r = row(threads, measured, label)
            rows[ver].append(r)
            return bad

        def tally(bad):
            nonlocal_counts[0] += bad
            nonlocal_counts[1] += 1 if bad else 0
            nonlocal_counts[2] += 1

        print("[%s] serial" % inst, flush=True)
        tally(record("serial", 1, best([SERIAL, infile], reference), "serial"))
        for p in PARALLELISM:
            print("[%s] omp t=%d" % (inst, p), flush=True)
            tally(record("omp", p,
                         best([OMP, infile], reference,
                              {"OMP_NUM_THREADS": str(p)}), "omp t=%d" % p))
        for p in PARALLELISM:
            print("[%s] mpi np=%d" % (inst, p), flush=True)
            tally(record("mpi", p,
                         best([MPIRUN, "-np", str(p), MPI, infile],
                              reference), "mpi np=%d" % p))
        # The hybrid is measured with threads live, and never oversubscribed:
        # processes x threads must stay within the core count.
        for p in [p for p in PARALLELISM if p * HYBRID_THREADS <= NCPU]:
            print("[%s] hybrid np=%d t=%d" % (inst, p, HYBRID_THREADS), flush=True)
            tally(record("hybrid", p,
                         best([MPIRUN, "-np", str(p), HYBRID, infile], reference,
                              {"OMP_NUM_THREADS": str(HYBRID_THREADS)}),
                         "hybrid np=%d t=%d" % (p, HYBRID_THREADS)))

    os.makedirs(OUTDIR, exist_ok=True)
    for ver in ("serial", "omp", "mpi", "hybrid"):
        path = os.path.join(OUTDIR, "comparison.%s.csv" % ver)
        with open(path, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(HEADER)
            w.writerows(rows[ver])
        print("wrote %s (%d rows)" % (path, len(rows[ver])), flush=True)
    bad_total, bad_runs, total_runs = nonlocal_counts
    print("wrote %s" % write_config(bad_total), flush=True)
    print("cores=%d parallelism=%s reps=%d hybridThreads=%d"
          % (NCPU, PARALLELISM, REPS, HYBRID_THREADS), flush=True)

    if bad_total:
        raise SystemExit("FAILED: %d of %d run(s) did not reproduce the reference "
                         "output (%d differing recommendation(s) in total); the data "
                         "records the mismatches"
                         % (bad_runs, total_runs, bad_total))
    print("all runs reproduced their reference output", flush=True)


if __name__ == "__main__":
    main()
