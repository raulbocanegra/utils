cmake_minimum_required(VERSION 3.0)

project(utils_test C CXX)

# test_optional definitions
add_executable(optional_tests ${PROJECT_SOURCE_DIR}/test_optional.cpp)
target_include_directories(optional_tests PUBLIC ${CATCH_INCLUDE_DIR})
target_link_libraries(optional_tests PRIVATE common)
add_test(NAME optional_tests COMMAND optional_tests)

# test_thread definitions
add_executable(thread_tests ${PROJECT_SOURCE_DIR}/test_thread.cpp)
target_include_directories(thread_tests PUBLIC ${CATCH_INCLUDE_DIR})
target_link_libraries(thread_tests PRIVATE common thread)
add_test(NAME thread_tests COMMAND thread_tests)

# test_scheduler definitions
add_executable(scheduler_tests ${PROJECT_SOURCE_DIR}/test_scheduler.cpp)
target_include_directories(scheduler_tests PUBLIC ${CATCH_INCLUDE_DIR})
target_link_libraries(scheduler_tests PRIVATE scheduler)
add_test(NAME scheduler_tests COMMAND scheduler_tests)
