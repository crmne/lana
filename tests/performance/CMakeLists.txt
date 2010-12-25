if(NOT EXISTS ${PROJECT_BINARY_DIR}/graphs.tar.bz2)
  message(STATUS "Downloading test graphs")
  file(DOWNLOAD 
    http://dl.dropbox.com/u/2729268/graphs.tar.bz2
    ${PROJECT_BINARY_DIR}/graphs.tar.bz2
    # EXPECTED_MD5 a936d91d925f2a8b1d90e883aaf4b262 # CMake 2.8 only
    )
  message(STATUS "Downloading test graphs - done")
endif() 

add_custom_command(
  OUTPUT graph.csv graph.metis
  COMMAND bunzip2 -kf ${PROJECT_BINARY_DIR}/graphs.tar.bz2 # workaround CMake 2.6-patch2 tar bug
  COMMAND ${CMAKE_COMMAND} -E tar xf ${PROJECT_BINARY_DIR}/graphs.tar
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  MAIN_DEPENDENCY ${PROJECT_BINARY_DIR}/graphs.tar.bz2
)

# TODO: move add_executable here
# TODO: rename add_boost_performance_test_executable to add_boost_performance_test
add_boost_performance_test_executable(page_rank csv ${MPI_LIBRARIES})
