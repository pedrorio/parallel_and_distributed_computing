# CTest driver, invoked via `cmake -P`. Runs the MPI solver under mpirun and
# checks its printed recommendations.
#
# Required -D variables:
#   MPIEXEC   path to mpirun/mpiexec
#   NPFLAG    the "number of processes" flag (e.g. -np)
#   NP        number of processes to launch
#   BIN       path to the matFact-mpi executable
#   INPUT     path to the .in instance
# Exactly one of:
#   EXPECTED  path to the .out file; the run must reproduce it
#   NP2       a second process count; both runs must produce identical output
# Optional:
#   OVERSUB   extra mpirun flag (e.g. --oversubscribe); ignored when empty
#
# Output is compared as a list of whitespace-separated integer tokens, so
# trailing blank lines or spacing differences never cause spurious failures.

function(run_solver np out_var)
    set(cmd "${MPIEXEC}")
    if(OVERSUB)
        list(APPEND cmd "${OVERSUB}")
    endif()
    list(APPEND cmd "${NPFLAG}" "${np}" "${BIN}" "${INPUT}")

    execute_process(
            COMMAND ${cmd}
            OUTPUT_VARIABLE out
            ERROR_VARIABLE err
            RESULT_VARIABLE rc
    )
    if(NOT rc EQUAL 0)
        message(FATAL_ERROR "mpirun (${np} proc) failed (rc=${rc}) on ${INPUT}\n${err}")
    endif()

    string(REGEX REPLACE "[ \t\r\n]+" ";" toks "${out}")
    list(FILTER toks EXCLUDE REGEX "^$")
    set(${out_var} "${toks}" PARENT_SCOPE)
endfunction()

run_solver("${NP}" actual)

if(DEFINED EXPECTED)
    file(READ "${EXPECTED}" ref_raw)
    string(REGEX REPLACE "[ \t\r\n]+" ";" ref "${ref_raw}")
    list(FILTER ref EXCLUDE REGEX "^$")

    list(LENGTH actual n_actual)
    list(LENGTH ref n_ref)
    if(NOT actual STREQUAL ref)
        message(FATAL_ERROR
                "Recommendation mismatch for ${INPUT} at ${NP} proc(s).\n"
                "  expected ${n_ref} values from ${EXPECTED}\n"
                "  got      ${n_actual} values\n"
                "  expected: ${ref}\n"
                "  actual:   ${actual}")
    endif()
elseif(DEFINED NP2)
    run_solver("${NP2}" actual2)
    if(NOT actual STREQUAL actual2)
        message(FATAL_ERROR
                "Output of ${INPUT} depends on process count (must not): "
                "${NP} proc != ${NP2} proc.\n"
                "  ${NP}:  ${actual}\n"
                "  ${NP2}: ${actual2}")
    endif()
else()
    message(FATAL_ERROR "run_compare.cmake: pass either -DEXPECTED=<file> or -DNP2=<n>")
endif()
