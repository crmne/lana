include_directories(
    ${Boost_INCLUDE_DIRS}
    ${MPI_INCLUDE_PATH}
    ${PROJECT_SOURCE_DIR}/src
)

# Include boost helper to generate executable and test automatically
include(MacroAddBoostTestExecutable)

add_boost_test_executable(csv csv)
add_boost_test_executable(degree_centrality ${MPI_LIBRARIES})
add_boost_test_executable(node_strength ${MPI_LIBRARIES})

if(WITH_PERFORMANCE_TESTING)
  add_subdirectory(performance)
endif()
