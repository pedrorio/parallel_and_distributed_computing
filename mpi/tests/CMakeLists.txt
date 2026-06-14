# Integration tests for the MPI solver, driven by CTest.
#
# Unlike the serial/omp suites (which call the in-process matFact() under
# GoogleTest), the MPI program is a real multi-process executable: it must be
# launched with mpirun and produces output only on the root rank. So each test
# runs `mpirun -np N matFact-mpi <instance>` and checks that:
#   * the printed recommendations match the course-provided <instance>.out, and
#   * the output is identical regardless of the number of processes.
# No GoogleTest dependency is required.

find_package(MPI REQUIRED)

set(INSTANCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../instances")
set(DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/run_compare.cmake")

# Oversubscribe when the launcher is Open MPI, so tests asking for more ranks
# than physical cores still run on small machines / CI.
set(OVERSUB "")
if(MPIEXEC_EXECUTABLE)
    execute_process(
            COMMAND "${MPIEXEC_EXECUTABLE}" --version
            OUTPUT_VARIABLE _mpi_ver ERROR_VARIABLE _mpi_ver)
    if(_mpi_ver MATCHES "Open MPI")
        set(OVERSUB "--oversubscribe")
    endif()
endif()

set(MPI_INSTANCES
        inst0
        inst1
        inst2
        inst30-40-10-2-10
        inst500-500-20-2-100
        inst1000-1000-100-2-30
        inst200-10000-50-100-300)

set(MPI_NPROCS 1 2 4)

# Correctness: every instance must reproduce its .out at each process count.
foreach(inst ${MPI_INSTANCES})
    foreach(np ${MPI_NPROCS})
        add_test(
                NAME mpi.${inst}.np${np}
                COMMAND ${CMAKE_COMMAND}
                    -DMPIEXEC=${MPIEXEC_EXECUTABLE}
                    -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
                    -DNP=${np}
                    -DOVERSUB=${OVERSUB}
                    -DBIN=$<TARGET_FILE:matFact-mpi>
                    -DINPUT=${INSTANCES_DIR}/${inst}.in
                    -DEXPECTED=${INSTANCES_DIR}/${inst}.out
                    -P ${DRIVER})
    endforeach()
endforeach()

# Determinism: the result must not depend on how many processes ran it.
foreach(inst inst0 inst500-500-20-2-100 inst1000-1000-100-2-30)
    add_test(
            NAME mpi.${inst}.deterministic.np1_vs_np4
            COMMAND ${CMAKE_COMMAND}
                -DMPIEXEC=${MPIEXEC_EXECUTABLE}
                -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
                -DNP=1
                -DNP2=4
                -DOVERSUB=${OVERSUB}
                -DBIN=$<TARGET_FILE:matFact-mpi>
                -DINPUT=${INSTANCES_DIR}/${inst}.in
                -P ${DRIVER})
endforeach()

# Edge case: more processes than there are non-zero entries (inst0 has 7).
add_test(
        NAME mpi.inst0.more_procs_than_nonzeros.np16
        COMMAND ${CMAKE_COMMAND}
            -DMPIEXEC=${MPIEXEC_EXECUTABLE}
            -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
            -DNP=16
            -DOVERSUB=${OVERSUB}
            -DBIN=$<TARGET_FILE:matFact-mpi>
            -DINPUT=${INSTANCES_DIR}/inst0.in
            -DEXPECTED=${INSTANCES_DIR}/inst0.out
            -P ${DRIVER})

# Robustness: bad input must make every rank abort cleanly, never deadlock.
# Each test runs with multiple ranks; WILL_FAIL expects a non-zero exit, and the
# TIMEOUT turns a hang (the bug these guard against) into a test failure.
set(MPIRUN_LAUNCH "${MPIEXEC_EXECUTABLE}")
if(OVERSUB)
    list(APPEND MPIRUN_LAUNCH "${OVERSUB}")
endif()
list(APPEND MPIRUN_LAUNCH "${MPIEXEC_NUMPROC_FLAG}" 4)

set(MALFORMED "${CMAKE_CURRENT_BINARY_DIR}/malformed.in")
file(WRITE "${MALFORMED}" "this is not\na valid instance\n")

add_test(NAME mpi.error.missing_argument
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi>)
add_test(NAME mpi.error.missing_file
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi> ${INSTANCES_DIR}/does_not_exist.in)
add_test(NAME mpi.error.malformed_file
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi> ${MALFORMED})

set_tests_properties(
        mpi.error.missing_argument
        mpi.error.missing_file
        mpi.error.malformed_file
        PROPERTIES WILL_FAIL TRUE TIMEOUT 30)
