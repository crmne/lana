find_package(Boost 1.35 COMPONENTS graph REQUIRED)
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
)
