#!/usr/bin/env python3
"""Measure serial / OpenMP / MPI wall-clock time across worker counts and write
compare/data/comparison.{serial,omp,mpi}.csv in the project's schema.

The whole end-to-end run time is recorded in the `loop` phase column (the
dominant phase for these compute-bound instances) so that graphs.R, which derives
total = readInput + initialLR + loop + finalFiltering, reproduces it.

Environment overrides (all optional):
  SERIAL_BIN, OMP_BIN, MPI_BIN   paths to the three executables
  MPIRUN                          mpirun/mpiexec launcher (default: mpirun)
  INSTANCES_DIR, OUT_DIR          input instances / output csv directory
  INSTANCES                       space-separated instance names (no extension)
  PARALLELISM                     space-separated worker counts (capped to #cores)
  REPS                            repetitions per measurement; the minimum is kept
"""
import csv, os, subprocess, time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


def env(key, default):
    return os.environ.get(key, default)


SERIAL = env("SERIAL_BIN", "/tmp/mf_serial")
OMP = env("OMP_BIN", "/tmp/mf_omp")
MPI = env("MPI_BIN", "/tmp/mf_mpi")
MPIRUN = env("MPIRUN", "mpirun")
INSTDIR = env("INSTANCES_DIR", os.path.join(ROOT, "instances"))
OUTDIR = env("OUT_DIR", os.path.join(ROOT, "compare", "data"))
REPS = int(env("REPS", "3"))
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


def best(cmd, extra_env=None):
    e = dict(os.environ)
    if extra_env:
        e.update(extra_env)
    times = []
    for _ in range(REPS):
        start = time.perf_counter()
        r = subprocess.run(cmd, stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL, env=e)
        if r.returncode != 0:
            raise SystemExit("command failed (%d): %s" % (r.returncode, " ".join(cmd)))
        times.append(time.perf_counter() - start)
    return min(times)


def main():
    rows = {"serial": [], "omp": [], "mpi": []}
    for inst in INSTANCES:
        d = dims(inst)
        infile = os.path.join(INSTDIR, inst + ".in")
        name = "../instances/%s.in" % inst

        def row(threads, sec):
            return [name, threads, 0, d["users"], d["items"], d["feats"],
                    d["nnz"], d["iters"], 0, 0, round(sec, 4), 0, round(sec, 4)]

        print("[%s] serial" % inst, flush=True)
        rows["serial"].append(row(1, best([SERIAL, infile])))
        for p in PARALLELISM:
            print("[%s] omp t=%d" % (inst, p), flush=True)
            rows["omp"].append(row(p, best([OMP, infile], {"OMP_NUM_THREADS": str(p)})))
        for p in PARALLELISM:
            print("[%s] mpi np=%d" % (inst, p), flush=True)
            rows["mpi"].append(row(p, best([MPIRUN, "-np", str(p), MPI, infile])))

    os.makedirs(OUTDIR, exist_ok=True)
    for ver in ("serial", "omp", "mpi"):
        path = os.path.join(OUTDIR, "comparison.%s.csv" % ver)
        with open(path, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(HEADER)
            w.writerows(rows[ver])
        print("wrote %s (%d rows)" % (path, len(rows[ver])), flush=True)
    print("cores=%d parallelism=%s reps=%d" % (NCPU, PARALLELISM, REPS), flush=True)


if __name__ == "__main__":
    main()
