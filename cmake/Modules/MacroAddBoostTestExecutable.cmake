# Helper that compiles and executes unit tests.
#
# Follows those naming conventions:
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
# UNIT_TEST_PROCESSES specifies the number of processes to run in parallel.
macro(add_boost_unit_test_executable TEST_NAME)
  add_executable(
    ${TEST_NAME}_unit
    ${TEST_NAME}.cpp
  )
  target_link_libraries(
    ${TEST_NAME}_unit
    ${Boost_LIBRARIES}
    ${ARGN} # arguments past the last expected one
  )
  
  # defaults to log all
  if(NOT DEFINED BOOST_TEST_LOG_OPTIONS)
    set(BOOST_TEST_LOG_OPTIONS --log_level=all --report_level=no)
  endif()
  add_test(
    ${TEST_NAME}_unit # name of the test
    ${TEST_NAME}_unit # name of the executable
    ${BOOST_TEST_LOG_OPTIONS}
  )
  if("${ARGN}" MATCHES "${MPI_LIBRARIES}")
    # defaults to 2 processes
    if(NOT DEFINED UNIT_TEST_PROCESSES)
      set(UNIT_TEST_PROCESSES 2)
    endif()
    
    add_test(
      ${TEST_NAME}_unit_parallel
      ${MPIEXEC}
      ${MPIEXEC_NUMPROC_FLAG}
      ${UNIT_TEST_PROCESSES} # number of processes to use
      ${MPIEXEC_PREFLAGS}
      ${TEST_NAME}_unit
      ${MPIEXEC_POSTFLAGS}
      ${BOOST_TEST_LOG_OPTIONS} # arguments to the program
    )
  endif()  
endmacro(add_boost_unit_test_executable)

macro(add_performance_test_executable TEST_NAME)
  add_executable(
    ${TEST_NAME}_performance
    benchmark.cpp
    benchmark.hpp
    ${TEST_NAME}.cpp
    utils.hpp
    graph.csv
    graph.metis
  )
  
  target_link_libraries(
    ${TEST_NAME}_performance
    ${Boost_LIBRARIES}
    ${MPI_LIBRARIES}
    ${ARGN}
  )
  
  if(NOT DEFINED PERFORMANCE_TEST_SERVER_LIST)
    set(PERFORMANCE_TEST_SERVER_LIST "$(./list_csunibo.rb)")
  endif()
  if(NOT DEFINED ${TEST_NAME}_MAX_PROCESSES)
    set(${TEST_NAME}_MAX_PROCESSES 10)
  endif()
  if(PERFORMANCE_TEST_SAME_SERVERS)
    set(__SAME_SERVERS_FLAG "-S")
  endif()

  add_test(
    ${TEST_NAME}_performance
    ./plot_speedup.rb
    -s ${PERFORMANCE_TEST_SERVER_LIST}
    -c ${TEST_NAME}_performance
    -p ${${TEST_NAME}_MAX_PROCESSES}
    ${__SAME_SERVERS_FLAG}
  )
endmacro(add_performance_test_executable)
