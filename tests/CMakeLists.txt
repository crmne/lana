include_directories(
    ${Boost_INCLUDE_DIRS}
    ${MPI_INCLUDE_PATH}
    ${PROJECT_SOURCE_DIR}/src
)

# Include boost helper to generate executable and test automatically
include(MacroAddBoostTestExecutable)

if(WITH_TESTING)
    enable_testing()
    add_subdirectory(unit)
endif()

if(WITH_PERFORMANCE_TESTING)
    enable_testing()
    add_subdirectory(performance)
endif()
