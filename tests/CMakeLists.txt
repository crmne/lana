include_directories(
    ${Boost_INCLUDE_DIRS}
    ${MPI_INCLUDE_PATH}
    ${PROJECT_SOURCE_DIR}/src
)

set(BOOST_TEST_LOG_OPTIONS --log_level=all --report_level=no)

# Helper for compiling and testing executables that uses our naming conventions
function(add_boost_test_executable TEST_NAME OTHER_LIBS)
    add_executable(test_${TEST_NAME} test_${TEST_NAME}.cpp)
    target_link_libraries(test_${TEST_NAME} ${Boost_LIBRARIES} ${OTHER_LIBS})

    add_test(${TEST_NAME} test_${TEST_NAME} ${BOOST_TEST_LOG_OPTIONS})
    if("${OTHER_LIBS}" MATCHES "${MPI_LIBRARIES}")
        add_test(${TEST_NAME}_parallel ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_${TEST_NAME} ${MPIEXEC_POSTFLAGS} ${BOOST_TEST_LOG_OPTIONS})
    endif()
endfunction(add_boost_test_executable)

add_boost_test_executable(csv csv)
add_boost_test_executable(degree_centrality "${MPI_LIBRARIES}")
add_boost_test_executable(node_strength "${MPI_LIBRARIES}")
