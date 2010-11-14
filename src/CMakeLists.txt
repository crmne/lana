# search for an MPI distribution
find_package(MPI REQUIRED)

# search for the Parallel Boost Graph Library and its dependencies
find_package(Boost 1.35 REQUIRED COMPONENTS
    graph_parallel
    mpi
    serialization
    system
    program_options
)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${MPI_INCLUDE_PATH}
)

add_library(
    csv
    csv.cpp
    csv.hpp # headers are useful with IDEs, and ignored by the Makefile generator
)

add_executable(
    mpsna
    cli.cpp
    degree_centrality.hpp
    graph_loaders.hpp
    social_graph.hpp
)
target_link_libraries(
    mpsna
    csv
    ${Boost_LIBRARIES}
    ${MPI_LIBRARIES}
)
