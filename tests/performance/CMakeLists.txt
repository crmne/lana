if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/graphs.tar.bz2)
  message(STATUS "Downloading test graphs")
  file(DOWNLOAD 
    http://dl.dropbox.com/u/2729268/graphs.tar.bz2
    ${CMAKE_CURRENT_BINARY_DIR}/graphs.tar.bz2
    # EXPECTED_MD5 a936d91d925f2a8b1d90e883aaf4b262 # CMake 2.8 only
    )
  message(STATUS "Downloading test graphs - done")
endif() 

add_custom_command(
  OUTPUT graph.csv graph.metis
  COMMAND bunzip2 -kf graphs.tar.bz2 # workaround CMake 2.6-patch2 tar bug
  COMMAND ${CMAKE_COMMAND} -E tar xf graphs.tar
  MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/graphs.tar.bz2
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/mpi_config.rb
  "MPIEXEC = '${MPIEXEC}'
MPIEXEC_NUMPROC_FLAG = '${MPIEXEC_NUMPROC_FLAG}'
MPIEXEC_PREFLAGS = '${MPIEXEC_PREFLAGS}'
MPIEXEC_POSTFLAGS = '${MPIEXEC_POSTFLAGS}'
"
)

# file(COPY . DESTINATION ${PROJECT_BINARY_DIR} PATTERN "*.rb" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE) # CMake 2.8 only
file(GLOB scripts RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.rb)
foreach(script ${scripts})
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${script} ${CMAKE_CURRENT_BINARY_DIR}/${script})
endforeach(script)

add_performance_test_executable(page_rank csv)
add_performance_test_executable(degree_centrality csv)
# add_performance_test_executable(node_strength csv)
