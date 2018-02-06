cmake_minimum_required(VERSION 3.0)

project(utils_test C CXX)

# test_optional definitions
add_executable(optional_tests ${PROJECT_SOURCE_DIR}/test_optional.cpp)
target_include_directories(optional_tests PUBLIC ${CATCH_INCLUDE_DIR} )
target_link_libraries(optional_tests PRIVATE utils)
add_test(NAME optional_tests COMMAND optional_tests)

# test_threading definitions
add_executable(threading_tests ${PROJECT_SOURCE_DIR}/test_threading.cpp)
target_include_directories(threading_tests PUBLIC ${CATCH_INCLUDE_DIR} )
target_link_libraries(threading_tests PRIVATE utils)
add_test(NAME threading_tests COMMAND threading_tests)
