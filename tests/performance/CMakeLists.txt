message(STATUS "Downloading test graphs")
file(DOWNLOAD 
  http://dl.dropbox.com/u/2729268/graphs.tar.bz2
  ${PROJECT_BINARY_DIR}/graphs.tar.bz2
  EXPECTED_MD5 a936d91d925f2a8b1d90e883aaf4b262
)
message(STATUS "Downloading test graphs - done")

add_custom_command(
  OUTPUT graph.csv graph.metis
  COMMAND bunzip2 ARGS -fk ${PROJECT_BINARY_DIR}/graphs.tar.bz2
  COMMAND tar ARGS xf ${PROJECT_BINARY_DIR}/graphs.tar
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  MAIN_DEPENDENCY ${PROJECT_BINARY_DIR}/graphs.tar.bz2
)

add_boost_performance_test_executable(page_rank csv ${MPI_LIBRARIES})
