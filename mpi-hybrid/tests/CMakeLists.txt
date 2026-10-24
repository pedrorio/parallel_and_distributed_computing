# Integration tests for the 2D-grid hybrid MPI+OpenMP solver, driven by CTest.
#
# Same approach as mpi/tests: the program is a real multi-process executable, so
# each test launches `mpirun -np N matFact-mpi-hybrid <instance>` and checks that the
# printed recommendations match the course-provided <instance>.out and are
# invariant to the process count. The grid solver additionally has a 2D process
# mesh and OpenMP threads, so this suite also covers non-square grids, empty
# blocks (more ranks than users/items), and the hybrid path with OMP_NUM_THREADS
# pinned. No GoogleTest dependency is required.

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

# Register a "reproduce <instance>.out at NP procs" test, optionally pinning
# OMP_NUM_THREADS to exercise the hybrid path.
function(mpi_compare_test name np inst)
    cmake_parse_arguments(T "" "THREADS" "" ${ARGN})
    add_test(
            NAME ${name}
            COMMAND ${CMAKE_COMMAND}
                -DMPIEXEC=${MPIEXEC_EXECUTABLE}
                -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
                -DNP=${np}
                -DOVERSUB=${OVERSUB}
                -DBIN=$<TARGET_FILE:matFact-mpi-hybrid>
                -DINPUT=${INSTANCES_DIR}/${inst}.in
                -DEXPECTED=${INSTANCES_DIR}/${inst}.out
                -P ${DRIVER})
    if(T_THREADS)
        set_tests_properties(${name} PROPERTIES ENVIRONMENT "OMP_NUM_THREADS=${T_THREADS}")
    endif()
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
        mpi_compare_test(hybrid.${inst}.np${np} ${np} ${inst})
    endforeach()
endforeach()

# Grid shapes: non-square (np3=3x1, np8=4x2) and a perfect square (np9=3x3).
foreach(np 3 8 9)
    mpi_compare_test(hybrid.shape.np${np} ${np} inst1000-1000-100-2-30)
endforeach()

# Empty blocks: more ranks than users/items (np16=4x4 on tiny inst0, 3 users).
# np8 is a 4x2 grid, so grid row 0 already owns zero users -- the cheapest case
# that exercises an empty block, and the one that caught the zero-length
# array-section reduction in updateLR under libgomp.
mpi_compare_test(hybrid.empty_blocks.np8  8  inst0)
mpi_compare_test(hybrid.empty_blocks.np16 16 inst0)

# Hybrid path: pin OMP_NUM_THREADS so each rank actually forks threads; the
# output must still match .out (invariance to thread count and to the FP
# reordering the per-thread reduction introduces).
mpi_compare_test(hybrid.hybrid.np1.t4  1  inst500-500-20-2-100   THREADS 4)
mpi_compare_test(hybrid.hybrid.np2.t2  2  inst1000-1000-100-2-30 THREADS 2)
mpi_compare_test(hybrid.hybrid.np2.t4  2  inst1000-1000-100-2-30 THREADS 4)
mpi_compare_test(hybrid.hybrid.empty_blocks.np16.t2 16 inst0     THREADS 2)

# Determinism: the result must not depend on how many processes ran it.
foreach(inst inst0 inst500-500-20-2-100 inst1000-1000-100-2-30)
    add_test(
            NAME hybrid.${inst}.deterministic.np1_vs_np4
            COMMAND ${CMAKE_COMMAND}
                -DMPIEXEC=${MPIEXEC_EXECUTABLE}
                -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
                -DNP=1
                -DNP2=4
                -DOVERSUB=${OVERSUB}
                -DBIN=$<TARGET_FILE:matFact-mpi-hybrid>
                -DINPUT=${INSTANCES_DIR}/${inst}.in
                -P ${DRIVER})
endforeach()

# Run-to-run stability of the OUTPUT at a fixed configuration: the same process and
# thread count must print the same recommendations twice. Threads are pinned so the
# per-thread reduction is live.
#
# Note what this does and does not assert. The computation itself is NOT bitwise
# reproducible with threads on: the per-thread reduction privates are combined in
# whatever order threads finish, which OpenMP does not specify, so L and R differ in
# their last bits between runs (measured: 8 distinct L/R checksums in 8 runs at
# np2/T4, under schedule(static) and schedule(guided) alike -- the loop schedule does
# not control it). The recommendations are integer argmax indices and are insensitive
# to that drift, which is why this holds. It is the output contract that matters here;
# a bitwise-reproducible gradient would need a fixed-order reduction instead.
add_test(
        NAME hybrid.repeatable.np2.t4
        COMMAND ${CMAKE_COMMAND}
            -DMPIEXEC=${MPIEXEC_EXECUTABLE}
            -DNPFLAG=${MPIEXEC_NUMPROC_FLAG}
            -DNP=2
            -DNP2=2
            -DOVERSUB=${OVERSUB}
            -DBIN=$<TARGET_FILE:matFact-mpi-hybrid>
            -DINPUT=${INSTANCES_DIR}/inst1000-1000-100-2-30.in
            -P ${DRIVER})
set_tests_properties(hybrid.repeatable.np2.t4 PROPERTIES ENVIRONMENT "OMP_NUM_THREADS=4")

# Robustness: bad input must make every rank abort cleanly, never deadlock.
# WILL_FAIL expects a non-zero exit; the TIMEOUT turns a hang into a failure.
set(MPIRUN_LAUNCH "${MPIEXEC_EXECUTABLE}")
if(OVERSUB)
    list(APPEND MPIRUN_LAUNCH "${OVERSUB}")
endif()
list(APPEND MPIRUN_LAUNCH "${MPIEXEC_NUMPROC_FLAG}" 4)

set(MALFORMED "${CMAKE_CURRENT_BINARY_DIR}/malformed.in")
file(WRITE "${MALFORMED}" "this is not\na valid instance\n")

add_test(NAME hybrid.error.missing_argument
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi-hybrid>)
add_test(NAME hybrid.error.missing_file
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi-hybrid> ${INSTANCES_DIR}/does_not_exist.in)
add_test(NAME hybrid.error.malformed_file
        COMMAND ${MPIRUN_LAUNCH} $<TARGET_FILE:matFact-mpi-hybrid> ${MALFORMED})

set_tests_properties(
        hybrid.error.missing_argument
        hybrid.error.missing_file
        hybrid.error.malformed_file
        PROPERTIES WILL_FAIL TRUE TIMEOUT 30)
