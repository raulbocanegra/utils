cmake_minimum_required(VERSION 3.0)

project(utils_test C CXX)

# Header and sources
set(UTILS_TEST_SOURCES ${PROJECT_SOURCE_DIR}/Main.cpp)

# target definitions
add_executable(${PROJECT_NAME} ${UTILS_TEST_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CATCH_INCLUDE_DIR} )
target_link_libraries(${PROJECT_NAME} PRIVATE utils)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
