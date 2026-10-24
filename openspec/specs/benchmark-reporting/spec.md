# benchmark-reporting Specification

## Purpose

How the project's performance evidence is produced and what it is allowed to claim: which
configurations get measured, that every measured run is checked for correctness rather
than assumed correct, and that published data, plots, and report figures state which
machine and which code they describe.

## Requirements

### Requirement: Published performance data is reproducible by one command

The committed comparison data SHALL be regenerable by running the benchmark script
against the current code, with no manual editing of the data files. Data and the plots
derived from it SHALL be committed together so they cannot describe different code.

#### Scenario: Regenerating reproduces the committed data

- **WHEN** the benchmark script is run against the committed code at the recorded
  configuration
- **THEN** it writes the comparison data files, and the committed values agree with the
  new run within normal run-to-run timing variation

#### Scenario: Plots derive from the committed data

- **WHEN** the plotting script is run
- **THEN** every published plot is produced from the committed comparison data files and
  from no other source

#### Scenario: Code change without measurement is visible

- **WHEN** an implementation's performance-relevant code changes and the data is not
  regenerated
- **THEN** the data no longer describes the shipped code, and the change is not complete
  until the data is regenerated

### Requirement: Every measured run is verified for correctness

The benchmark SHALL compare each timed run's recommendations against the instance's
reference output and record the number of mismatches. A correctness value SHALL never be
assumed, hardcoded, or defaulted.

#### Scenario: Correct run is recorded as correct

- **WHEN** a timed run's output matches the instance's reference output
- **THEN** the recorded mismatch count for that row is zero

#### Scenario: Incorrect run is recorded, not silently timed

- **WHEN** a timed run's output differs from the reference output
- **THEN** the recorded mismatch count is non-zero and the discrepancy is surfaced rather
  than reported as a successful measurement

#### Scenario: Output is available to be checked

- **WHEN** a run is timed
- **THEN** its recommendations are captured for comparison, not discarded

### Requirement: Recorded timings mean what their columns say

Each recorded timing column SHALL correspond to something actually measured. A column
that is not measured SHALL NOT be populated with a placeholder value, and claims about a
breakdown of time SHALL only be made where that breakdown was recorded.

#### Scenario: Unmeasured phase columns

- **WHEN** the benchmark records only end-to-end wall-clock time
- **THEN** no per-phase figure is presented as measured data in the reports

#### Scenario: Timing methodology is stated

- **WHEN** performance data is published
- **THEN** the number of repetitions and how they are reduced to one figure is recorded
  alongside it

### Requirement: Measurement configuration is recorded with the data

Published data SHALL carry the configuration it was produced under: the instances
measured, the worker counts, and the core count of the machine that ran it.

#### Scenario: Configuration accompanies the data

- **WHEN** comparison data is committed
- **THEN** the worker counts and the measuring machine's core count are recorded with it

#### Scenario: Parallelism beyond the core count

- **WHEN** a requested worker count exceeds the measuring machine's available cores
- **THEN** that configuration is either skipped or recorded as oversubscribed, so
  oversubscribed timings are not presented as scaling results

### Requirement: Report figures state their provenance

Every table and plot in the reports SHALL identify the machine its figures were measured
on. Figures from the original course cluster and figures measured locally SHALL be
distinguishable, and figures that cannot be regenerated from this repository SHALL be
marked as such.

#### Scenario: Mixed-provenance table

- **WHEN** a report table presents both course-cluster rows and locally measured rows
- **THEN** each row identifies its machine, and the caption names both machines

#### Scenario: Figures for absent instances

- **WHEN** a report cites an instance that is not present in `instances/`
- **THEN** that figure is labelled as measured on the course cluster and not regenerable
  from this repository

#### Scenario: Cross-machine comparison is not implied

- **WHEN** cluster and local figures appear in the same report
- **THEN** no speedup, ratio, or conclusion is drawn by comparing a figure from one
  machine against a figure from the other

### Requirement: Report claims match the published data

A quantitative claim in a report SHALL be supported by the published data or by a clearly
labelled cluster measurement. Figures SHALL NOT be maintained only inside report source
where the data pipeline cannot check them.

#### Scenario: Claim contradicted by regenerated data

- **WHEN** regenerated data contradicts a report's narrative
- **THEN** the report is corrected to match the measurement, rather than the measurement
  being repeated until it agrees with the report

#### Scenario: Locally measured figure appears in a report

- **WHEN** a report presents a figure measured on this machine
- **THEN** that figure traces to a row in the committed comparison data
