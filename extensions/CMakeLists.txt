cmake_minimum_required (VERSION 3.8)
project (ai)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
include(${PROJECT_SOURCE_DIR}/CMakeModules/cxx_compiler_functions.cmake)

option(USE_64BIT_BUILD "USE_64BIT_BUILD" OFF)
option(USE_STATIC_LINKING "USE_STATIC_LINKING" ON)

set(CMAKE_CL_64 ${USE_64BIT_BUILD})
if(USE_STATIC_LINKING)
	message("WARNING: Linking statically")
	set(INTERCEPT_LINK_TYPE "static")
else()
	message("WARNING: Linking dynamically")
	set(INTERCEPT_LINK_TYPE "dynamic")
endif()

if("${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017")
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++latest" )
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	message(FATAL_ERROR "ERROR: You need a C++17 compatible compiler")
endif()

message("GENERATOR USED: '${CMAKE_GENERATOR}'")
message("COMPILER USED: '${CMAKE_CXX_COMPILER_ID}'")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


SET(CMAKE_INCLUDE_CURRENT_DIR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(USE_64BIT_BUILD)
    message("WARNING: Building 64-bit projects")
    set(ACRE_ARCH "x64")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build64/")
    link_directories("C:/SteamLibrary/_Working/addons/Dev/intercept/build/win64/intercept_client/Debug/intercept_client.lib")
else()
    message("WARNING: Building 32-bit projects")
    set(ACRE_ARCH "x86")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build32/")
    link_directories("C:/SteamLibrary/_Working/addons/Dev/intercept/build/win32/intercept_client/Debug/intercept_client.lib")
endif()

add_subdirectory(src/ai)
set(INTERCEPT_INCLUDE_DIR "C:/SteamLibrary/_Working/addons/Dev/intercept/src/client/headers")
