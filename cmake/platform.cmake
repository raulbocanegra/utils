# Set platform dependent properties
function(set_platform_properties)	
	if (UNIX)
		message("Setting properties for UNIX platform")
		add_compile_options(
			-m64 # 64 bits build flag
		)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" PARENT_SCOPE)
		set(CMAKE_CXX_STANDARD 11 PARENT_SCOPE) # C++11...
		set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE) #...is required...
		set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE) #...without compiler extensions like gnu++11
	elseif (WIN32 AND MSVC)
		message("Setting properties for Windows Visual Studio platform")
	    #set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_TOOLCHAIN_DIR}/vcpkg.cmake" CACHE PATH "vcpkg.cmake directory") 
		add_definitions(
			-DNOMINMAX # https://support.microsoft.com/en-gb/help/143208/prb-using-stl-in-windows-program-can-cause-min-max-conflicts
		)
		add_compile_options(
			/MP # https://msdn.microsoft.com/en-us/library/bb385193.aspx
		)
	else()
		message("Unknown Platform")
	endif()	
endfunction()