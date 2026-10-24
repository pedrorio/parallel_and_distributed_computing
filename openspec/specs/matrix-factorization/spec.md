# matrix-factorization Specification

## Purpose

The recommender's observable contract, shared by the serial, OpenMP, and MPI
implementations: how an instance is parsed and validated, what recommendations are
printed, and the guarantee that those recommendations do not depend on how much
parallelism was used to compute them.

## Requirements

### Requirement: Recommendation output reproduces the reference result

Every implementation SHALL print one recommended item index per user, one per line, in
ascending user order, and that output SHALL match the course-provided `<instance>.out`
file exactly for every instance in `instances/`.

#### Scenario: Serial run reproduces the reference output

- **WHEN** the serial solver is run on an instance in `instances/`
- **THEN** its standard output matches that instance's `.out` file byte for byte

#### Scenario: Reference output is the shared oracle

- **WHEN** any implementation's output is compared against the reference
- **THEN** the comparison is made against the committed `<instance>.out` file, not
  against another implementation's output

### Requirement: Output is invariant to the amount of parallelism

The recommendations SHALL be identical regardless of process count, process grid shape,
or thread count. Parallel configuration SHALL affect only running time and memory, never
the result.

#### Scenario: Varying process count

- **WHEN** the MPI solver is run on the same instance at 1, 2, and 4 processes
- **THEN** all three runs print identical recommendations, matching the `.out` file

#### Scenario: Non-square process grid

- **WHEN** the process count factors into a non-square grid, including a prime count
  that yields a single row or column
- **THEN** the recommendations are unchanged

#### Scenario: More processes than users or items

- **WHEN** the process count exceeds the instance's user or item count, so that at least
  one process owns an empty block
- **THEN** the run completes and the recommendations are unchanged

#### Scenario: Hybrid threads enabled

- **WHEN** the MPI solver is run with `OMP_NUM_THREADS` greater than one
- **THEN** the recommendations are unchanged from the single-threaded run

#### Scenario: Repeated run at a fixed configuration

- **WHEN** the same instance is run twice at the same process and thread count
- **THEN** both runs produce byte-identical output

### Requirement: Ties resolve to the lowest item index

When two or more items share the highest predicted value for a user, the implementation
SHALL recommend the lowest item index among them, matching the reference behavior at any
process count or grid shape.

#### Scenario: Tied predictions across process boundaries

- **WHEN** the tied items are owned by different processes
- **THEN** the recommended item is the lowest of the tied global item indices

#### Scenario: No positive prediction for a user

- **WHEN** no unmasked item has a predicted value above zero for a user
- **THEN** item index 0 is recommended for that user

### Requirement: Matrix initialisation is deterministic and independent of parallelism

The initial factor matrices SHALL be generated from a fixed seed in a single defined
order, so that every process derives exactly the values the serial implementation would
produce for the rows and columns it owns.

#### Scenario: Distributed initialisation matches serial

- **WHEN** the solver is run at any process count
- **THEN** each process's slice of the factor matrices holds the same values the serial
  implementation would compute at those positions

### Requirement: Malformed instances are rejected without hanging

An instance that is missing, unreadable, or inconsistent with its declared header SHALL
cause the run to fail with a diagnostic on standard error and a non-zero exit status. In
a distributed run, every process SHALL terminate; no process may block waiting for data
that will never be sent.

#### Scenario: Unreadable instance file

- **WHEN** the named instance file does not exist or cannot be opened
- **THEN** the run reports the failure on standard error and exits non-zero

#### Scenario: Header inconsistent with contents

- **WHEN** the declared non-zero count exceeds the number of entries present, or a
  dimension is non-positive
- **THEN** the run reports the failure and exits non-zero without printing
  recommendations

#### Scenario: Non-zero index out of range

- **WHEN** an entry names a user or item index outside the declared dimensions
- **THEN** the run reports the failure and exits non-zero

#### Scenario: Rejection in a multi-process run

- **WHEN** a malformed instance is given to a run with more than one process
- **THEN** every process terminates and the run does not deadlock

### Requirement: Memory per process falls as processes are added

The MPI implementation SHALL partition its working data across processes rather than
replicating it, so that per-process memory decreases as the process count grows. No
process may hold a dense copy of the whole prediction matrix.

#### Scenario: Per-process footprint shrinks

- **WHEN** the same instance is run at increasing process counts
- **THEN** each process's allocation for the factor matrices, the gradient, and the
  prediction block decreases

#### Scenario: Documented scaling matches the implementation

- **WHEN** the implementation's documentation states a memory bound
- **THEN** that bound accounts for every allocation that scales with the problem size,
  including the gradient accumulators, any per-thread private copies, and the dense
  prediction block
