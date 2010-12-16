# Helper for compiling and testing executables that uses our naming conventions:
# source file     = ${TEST_NAME}.cpp
# executable name = ${TEST_NAME}_${TEST_TYPE}
# test name       = ${TEST_NAME}_${TEST_TYPE}
#
# Arguments past the last expected one (TEST_TYPE) can be used to specify other libraries to link to.
# If the test links to MPI_LIBRARIES a special test named ${TEST_NAME}_${TEST_TYPE}_parallel will be generated,
# that will execute in parallel.
#
# Other variables:
# BOOST_TEST_LOG_OPTIONS specifies the options to pass to the test executables, see `tests/degree_centrality_unit --help` for more informations.
# MPIEXEC_NUMPROCS specifies the number of processes to run in parallel.
macro(__add_boost_test_executable TEST_NAME TEST_TYPE)
    add_executable(
        ${TEST_NAME}_${TEST_TYPE}
        ${TEST_NAME}.cpp
    )
    target_link_libraries(
        ${TEST_NAME}_${TEST_TYPE}
        ${Boost_LIBRARIES}
        ${ARGN} # arguments past the last expected one
    )

    # defaults to log all
    if(NOT DEFINED BOOST_TEST_LOG_OPTIONS)
        set(BOOST_TEST_LOG_OPTIONS --log_level=all --report_level=no)
    endif()
    add_test(
        ${TEST_NAME}_${TEST_TYPE} # name of the test
        ${TEST_NAME}_${TEST_TYPE} # name of the executable
        ${BOOST_TEST_LOG_OPTIONS}
    )
    if("${ARGN}" MATCHES "${MPI_LIBRARIES}")
        # defaults to 2 processes
        if(NOT DEFINED MPIEXEC_NUMPROCS)
            set(MPIEXEC_NUMPROCS 2)
        endif()

        add_test(
            ${TEST_NAME}_${TEST_TYPE}_parallel
            ${MPIEXEC}
            ${MPIEXEC_NUMPROC_FLAG}
            ${MPIEXEC_NUMPROCS} # number of processes to use
            ${MPIEXEC_PREFLAGS}
            ${TEST_NAME}_${TEST_TYPE}
            ${MPIEXEC_POSTFLAGS}
            ${BOOST_TEST_LOG_OPTIONS} # arguments to the program
    )
    endif()
endmacro(__add_boost_test_executable)

macro(add_boost_unit_test_executable TEST_NAME)
  __add_boost_test_executable(${TEST_NAME} unit ${ARGN})
endmacro(add_boost_unit_test_executable)

macro(add_boost_performance_test_executable TEST_NAME)
  __add_boost_test_executable(${TEST_NAME} performance benchmark ${ARGN})
endmacro(add_boost_performance_test_executable)
