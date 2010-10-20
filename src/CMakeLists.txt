# search for an MPI distribution
find_package(MPI REQUIRED)

# search for the Parallel Boost Graph Library and its dependencies
find_package(Boost 1.35 REQUIRED COMPONENTS
    graph_parallel
    mpi
    serialization
    system
)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_library(csv csv.cpp)

add_executable(
	sigsna_degree
	sigsna_degree.cpp
)
target_link_libraries(
	sigsna_degree
	csv
	${Boost_LIBRARIES}
	${MPI_LIBRARIES}
)
