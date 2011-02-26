project(lana CXX)

# Required cmake version
cmake_minimum_required(VERSION 2.6)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# add definitions
include(DefineCMakeDefaults)
include(DefineOptions)

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

# search for an MPI distribution
find_package(MPI REQUIRED)

# search for the Parallel Boost Graph Library and its dependencies
find_package(Boost 1.42 REQUIRED COMPONENTS
    graph_parallel
    mpi
    serialization
    system
    program_options
    date_time
    filesystem
    ${BOOST_TEST_DEPENDENCIES}
)

# check subdirectories
add_subdirectory(src)
add_subdirectory(tests)
