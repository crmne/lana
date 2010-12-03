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
    lana
    cli.cpp

    benchmark.cpp
    benchmark.hpp

    degree_centrality.hpp
    graph_loaders.hpp
    social_graph.hpp
)
target_link_libraries(
    lana
    csv
    ${Boost_LIBRARIES}
    ${MPI_LIBRARIES}
)
