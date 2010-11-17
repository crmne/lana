# Helper for compiling and testing executables that uses our naming conventions:
# source file     = test_${TEST_NAME}.cpp
# executable name = test_${TEST_NAME}
# test name       = ${TEST_NAME}
#
# Arguments past the last expected one (TEST_NAME) can be used to specify other libraries to link to.
# If the test links to MPI_LIBRARIES a special test named ${TEST_NAME}_parallel will be generated,
# that will execute in parallel.
#
# Other variables:
# BOOST_TEST_LOG_OPTIONS specifies the options to pass to the test executables, see `test_* --help` for more informations.
# MPIEXEC_NUMPROCS specifies the number of processes to run in parallel.
macro(add_boost_test_executable TEST_NAME)
    add_executable(
        test_${TEST_NAME}
        test_${TEST_NAME}.cpp
    )
    target_link_libraries(
        test_${TEST_NAME}
        ${Boost_LIBRARIES}
        ${ARGN} # arguments past the last expected one
    )

    # defaults to log all
    if(NOT DEFINED BOOST_TEST_LOG_OPTIONS)
        set(BOOST_TEST_LOG_OPTIONS --log_level=all --report_level=no)
    endif()
    add_test(
        ${TEST_NAME}
        test_${TEST_NAME}
        ${BOOST_TEST_LOG_OPTIONS}
    )
    if("${ARGN}" MATCHES "${MPI_LIBRARIES}")
        # defaults to 2 processes
        if(NOT DEFINED MPIEXEC_NUMPROCS)
            set(MPIEXEC_NUMPROCS 2)
        endif()

        add_test(
            ${TEST_NAME}_parallel
            ${MPIEXEC}
            ${MPIEXEC_NUMPROC_FLAG}
            ${MPIEXEC_NUMPROCS} # numbers of processes to use
            ${MPIEXEC_PREFLAGS}
            test_${TEST_NAME}
            ${MPIEXEC_POSTFLAGS}
            ${BOOST_TEST_LOG_OPTIONS} # arguments to the program
    )
    endif()
endmacro(add_boost_test_executable)