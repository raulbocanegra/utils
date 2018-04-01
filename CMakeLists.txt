cmake_minimum_required(VERSION 3.0)

project(utils C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/platform.cmake)

set_platform_properties()

option(USE_CONAN "Use Conan Package Manager to deal with dependencies" ON)
if(USE_CONAN)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

message("Generator: ${CMAKE_GENERATOR}")

# Set global platform variable for source directories
if (UNIX)
	set(PLATFORM "Linux")	
elseif (WIN32 AND MSVC)
	set(PLATFORM "Windows")	
endif()	

# Project options
option(BUILD_TESTS "Build tests" ON)

# Testing library include dir
set(CATCH_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/test/include")

# .cmake files
file(GLOB UTILS_CMAKE_SCRIPTS ${PROJECT_SOURCE_DIR}/cmake/*.cmake)

########################################################################################################################
# Target definitions
########################################################################################################################
# common
set(COMMON_HEADERS ${PROJECT_SOURCE_DIR}/common/include/common/Optional.h
                   ${PROJECT_SOURCE_DIR}/common/include/common/Value.h
                   ${PROJECT_SOURCE_DIR}/common/include/common/Utility.h)
add_library(common INTERFACE)
target_sources(common INTERFACE ${COMMON_HEADERS})
target_include_directories(common INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/common/include>)

# thread
set(THREAD_HEADERS ${PROJECT_SOURCE_DIR}/thread/include/thread/Threadpool.h
				   ${PROJECT_SOURCE_DIR}/thread/include/thread/ActiveWorker.h)
add_library(thread INTERFACE)
target_sources(thread INTERFACE ${THREAD_HEADERS})
target_include_directories(thread INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/thread/include>)
if(UNIX)
	target_link_libraries(thread INTERFACE pthread)
endif()
# scheduler

set(SCHEDULER_HEADERS ${PROJECT_SOURCE_DIR}/scheduler/include/scheduler/Scheduler.h)
set(SCHEDULER_SOURCES ${PROJECT_SOURCE_DIR}/scheduler/src/Scheduler.cpp
                      ${PROJECT_SOURCE_DIR}/scheduler/src/Scheduler${PLATFORM}.cpp)
add_library(scheduler ${SCHEDULER_HEADERS} ${SCHEDULER_SOURCES})
target_include_directories(scheduler PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/scheduler/include>)
if(UNIX)
	target_link_libraries(scheduler PRIVATE rt 
	                                INTERFACE pthread)
endif()

# Add executable (for manual testing)
add_executable(utils_exe ${PROJECT_SOURCE_DIR}/common/src/Main.cpp)
target_link_libraries(utils_exe scheduler)

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
    	set(DOXYGEN_GENERATE_HTML YES)
		set(DOXYGEN_GENERATE_LATEX YES)
		doxygen_add_docs(headers_doc ${UTILS_HEADERS} ${PROJECT_SOURCE_DIR}/Readme.md COMMENT "Generating API documentation with Doxygen")
		#doxygen_add_docs(user_doc ${PROJECT_SOURCE_DIR}/Readme.md COMMENT "Generating User documentation with Doxygen")
	else ()
		message("Doxygen needs to be installed to generate the doxygen documentation")
	endif ()
endif()

