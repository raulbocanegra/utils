cmake_minimum_required(VERSION 3.0)

project(utils C CXX)

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

# Project options
option(BUILD_TESTS "Build tests" ON)

# Testing library include dir
set(CATCH_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/test/include")

# .cmake files
file(GLOB UTILS_CMAKE_SCRIPTS ${PROJECT_SOURCE_DIR}/cmake/*.cmake)

# Header and sources
set(UTILS_HEADERS ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Optional.h
                  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Value.h
                  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/utility/Utility.h
                  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/threading/Threadpool.h
				  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/threading/ActiveWorker.h)

set(UTILS_SOURCES)

source_group("include\\${PROJECT_NAME}" FILES ${UTILS_HEADERS})
source_group("src" FILES ${UTILS_SOURCES})
source_group("cmake" FILES ${UTILS_CMAKE_SCRIPTS})

# target definitions
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${UTILS_HEADERS})
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> )

# Add executable (for manual testing)
add_executable(utils_exe ${PROJECT_SOURCE_DIR}/src/Main.cpp)
target_link_libraries(utils_exe ${PROJECT_NAME})


# Add subdirectories
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
if(BUILD_DOC)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
    
		message("DOXYGEN_EXECUTABLE=${DOXYGEN_EXECUTABLE}")
		set(DOXYGEN_GENERATE_HTML YES)
	
		doxygen_add_docs(doc_doxygen ${UTILS_HEADERS} COMMENT "Generating API documentation with Doxygen")
		
	else ()
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif ()
endif()

