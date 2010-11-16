# search for an MPI distribution
find_package(MPI REQUIRED)

# search for the Parallel Boost Graph Library and its dependencies
find_package(Boost 1.42 REQUIRED COMPONENTS
    graph_parallel
    mpi
    serialization
    system
    unit_test_framework
)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(test_csv test_csv.cpp)
target_link_libraries(test_csv ${Boost_LIBRARIES} csv)

add_executable(test_degree_centrality test_degree_centrality.cpp)
target_link_libraries(test_degree_centrality ${Boost_LIBRARIES} ${MPI_LIBRARIES})

set(BOOST_TEST_LOG_OPTIONS --log_level=all --report_level=no)

add_test(csv test_csv ${BOOST_TEST_LOG_OPTIONS})
add_test(degree_centrality test_degree_centrality ${BOOST_TEST_LOG_OPTIONS})
add_test(degree_centrality_parallel ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_degree_centrality ${MPIEXEC_POSTFLAGS} ${BOOST_TEST_LOG_OPTIONS})
