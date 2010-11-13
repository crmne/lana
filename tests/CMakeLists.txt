find_package(Boost 1.35 COMPONENTS unit_test_framework REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(test_csv test_csv.cpp)
target_link_libraries(test_csv ${Boost_LIBRARIES} csv)

add_test(test_csv ${EXECUTABLE_OUTPUT_PATH}/test_csv)
