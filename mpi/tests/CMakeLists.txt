# Integration tests for the pure MPI 2D-grid solver, driven by CTest.
#
# Same approach as mpi/tests: the program is a real multi-process executable, so
# each test launches `mpirun -np N matFact-mpi <instance>` and checks that the
# printed recommendations match the course-provided <instance>.out and are
# invariant to the process count. The solver arranges ranks as a 2D mesh, so this
# suite also covers non-square grids and empty blocks (more ranks than
# users/items). No GoogleTest dependency is required.
#
# Thread-dependent cases live in mpi-hybrid/tests/ -- this variant has no threads.

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
    if(_mpi_ver MATCHES "Open MPI" OR _mpi_ver MATCHES "OpenRTE")
        set(OVERSUB "--oversubscribe")
    endif()
endif()

# Register a "reproduce <instance>.out at NP procs" test.
function(mpi_compare_test name np inst)
    add_test(
            NAME ${name}
            COMMAND ${CMAKE_COMMAND}
                -DMPIEXEC=${MPIEXEC_EXECUTABLE}
                -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
                -DNP=${np}
                -DOVERSUB=${OVERSUB}
                -DBIN=$<TARGET_FILE:matFact-mpi>
                -DINPUT=${INSTANCES_DIR}/${inst}.in
                -DEXPECTED=${INSTANCES_DIR}/${inst}.out
                -P ${DRIVER})
endfunction()

set(GRID_INSTANCES
        inst0
        inst1
        inst2
        inst30-40-10-2-10
        inst500-500-20-2-100
        inst1000-1000-100-2-30
        inst200-10000-50-100-300)

# Correctness (pure MPI): every instance reproduces its .out at np 1/2/4.
foreach(inst ${GRID_INSTANCES})
    foreach(np 1 2 4)
        mpi_compare_test(mpi.${inst}.np${np} ${np} ${inst})
    endforeach()
endforeach()

# Grid shapes: non-square (np3=3x1, np8=4x2) and a perfect square (np9=3x3).
foreach(np 3 8 9)
    mpi_compare_test(mpi.shape.np${np} ${np} inst1000-1000-100-2-30)
endforeach()

# Empty blocks: more ranks than users/items (np16=4x4 on tiny inst0, 3 users).
# np8 is a 4x2 grid, so grid row 0 already owns zero users -- the cheapest case
# that exercises an empty block, and the one that caught the zero-length
# array-section reduction in updateLR under libgomp.
mpi_compare_test(mpi.empty_blocks.np8  8  inst0)
mpi_compare_test(mpi.empty_blocks.np16 16 inst0)

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

# Robustness: bad input must make every rank abort cleanly, never deadlock.
# WILL_FAIL expects a non-zero exit; the TIMEOUT turns a hang into a failure.
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
