cmake_minimum_required(VERSION 3.0)

project(utils CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/platform.cmake)

message("Build: ${CMAKE_GENERATOR}")

set_platform_properties()

# Set global platform variable for source directories
if (UNIX)
	set(PLATFORM "linux")	
elseif (WIN32 AND MSVC)
	set(PLATFORM "windows")	
endif()	

set(ENABLE_TESTING ON CACHE BOOL "Enable testing")
# Testing library include dir
set(CATCH_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/test/include")

# .cmake files
file(GLOB UTILS_CMAKE_SCRIPTS ${PROJECT_SOURCE_DIR}/cmake/*.cmake)

# Header and sources
set(UTILS_HEADERS ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Optional.h
                  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Value.h )

set(UTILS_SOURCES ${PROJECT_SOURCE_DIR}/src/Optional.cpp
                  ${PROJECT_SOURCE_DIR}/src/Value.cpp
                  ${PROJECT_SOURCE_DIR}/src/Main.cpp)

source_group("include\\${PROJECT_NAME}" FILES ${UTILS_HEADERS})
source_group("src" FILES ${UTILS_SOURCES})
source_group("cmake" FILES ${UTILS_CMAKE_SCRIPTS})

# target definitions
add_library(${PROJECT_NAME} ${UTILS_HEADERS} ${UTILS_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> )
#target_link_libraries(${PROJECT_NAME} )
#target_compile_definitions(${PROJECT_NAME} PUBLIC )

# Add subdirectories
if(ENABLE_TESTING)
	add_subdirectory(test)
endif()