add_executable(perftest_page_rank page_rank.cpp)
target_link_libraries(perftest_page_rank benchmark ${Boost_LIBRARIES} ${MPI_LIBRARIES})
add_test(page_rank_performance perftest_page_rank)
