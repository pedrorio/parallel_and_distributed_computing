# solver-variants Specification

## Purpose

Which solver implementations the project ships as deliverables, what makes each one
self-standing — its own build, its own tests, its own write-up — and what independently
maintained implementations must still agree on so they cannot silently diverge.

## Requirements

### Requirement: Each variant is a self-standing deliverable

Every shipped solver variant SHALL build, run and be tested from its own directory
without depending on another variant's sources, build files or test suite.

#### Scenario: Building one variant alone

- **WHEN** a variant's directory is configured and built on its own
- **THEN** it produces its executable without referencing another variant's sources

#### Scenario: Testing one variant alone

- **WHEN** a variant's test suite is run
- **THEN** it exercises that variant's executable only, and passes or fails on its own
  behaviour

#### Scenario: Removing one variant

- **WHEN** one variant's directory is removed
- **THEN** every other variant still builds and passes its tests

### Requirement: The pure-MPI variant carries no shared-memory threading

The variant presented as the pure MPI deliverable SHALL contain no OpenMP directives,
no thread-count handling, and no build dependency on an OpenMP runtime. Its parallelism
SHALL come from MPI processes alone.

#### Scenario: Building without an OpenMP runtime

- **WHEN** the pure-MPI variant is built in an environment with no OpenMP support
- **THEN** configuration and compilation succeed

#### Scenario: Thread settings have no effect

- **WHEN** the pure-MPI variant is run with a thread-count environment variable set to
  any value
- **THEN** the run behaves identically to running it unset, because nothing reads it

#### Scenario: Initialising MPI without thread support

- **WHEN** the pure-MPI variant starts
- **THEN** it initialises MPI without requesting a threading level

### Requirement: The hybrid variant combines MPI processes with threads

The variant presented as the hybrid deliverable SHALL parallelise within each process
using threads as well as across processes using MPI, SHALL request a threading level
compatible with how it calls MPI, and SHALL default to one thread per process so a run
never oversubscribes by accident.

#### Scenario: Threads opted into explicitly

- **WHEN** the hybrid variant is run with the thread count set above one
- **THEN** each process divides its local work across that many threads

#### Scenario: No thread count given

- **WHEN** the hybrid variant is run with no thread count set
- **THEN** it runs one thread per process

#### Scenario: MPI calls confined to one thread

- **WHEN** the hybrid variant performs its cross-process reductions
- **THEN** those calls are made by a single thread per process, consistent with the
  threading level it requested at start-up

### Requirement: Variants agree on every reference result

Every variant SHALL reproduce each instance's reference output. Agreement SHALL be
checked for each variant separately, so a change applied to one and not another is
detected rather than inferred.

#### Scenario: Both variants on the same instance

- **WHEN** each variant is run on the same instance
- **THEN** both reproduce that instance's reference output exactly

#### Scenario: A change applied to one variant only

- **WHEN** a behavioural change is made to one variant and not the other
- **THEN** at least one variant's test suite fails, identifying which one diverged

#### Scenario: Shared instances

- **WHEN** reference instances are used for verification
- **THEN** every variant verifies against the same shared instance files, not per-variant
  copies

### Requirement: Published evidence distinguishes the variants

Performance data, plots and reports SHALL attribute every figure to the variant that
produced it, and SHALL NOT present a figure from one variant as evidence about another.

#### Scenario: Data for each variant

- **WHEN** performance data is published
- **THEN** each variant's measurements are recorded separately and identifiably

#### Scenario: Comparing variants

- **WHEN** a report compares variants
- **THEN** each compared figure names the variant it came from, and the comparison is
  between figures measured the same way

#### Scenario: A variant that was not measured

- **WHEN** a variant has no measurement for a configuration
- **THEN** the gap is left visible rather than filled with another variant's figure

### Requirement: Each variant documents what distinguishes it

Every variant SHALL carry documentation stating what parallelism it uses, how to build
and run it, and how it differs from the sibling variants.

#### Scenario: Reading a variant's documentation

- **WHEN** a variant's documentation is read
- **THEN** it states that variant's parallelism model, its build and run commands, and
  what sets it apart from the others

#### Scenario: Tuning guidance belongs to the variant it applies to

- **WHEN** guidance concerns a mechanism only one variant has
- **THEN** that guidance appears in that variant's documentation and not in a variant
  that lacks the mechanism
