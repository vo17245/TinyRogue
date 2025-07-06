cmake_minimum_required(VERSION 3.10)
project(TinyRogue)
if(MSVC)
    include("${CMAKE_SOURCE_DIR}/CMake/VsConfig.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
file(GLOB_RECURSE FILES
"${CMAKE_SOURCE_DIR}/Source/*.cpp" 
"${CMAKE_SOURCE_DIR}/Source/*.h"
)
set(CMAKE_CXX_STANDARD 23)
set(AETHER_ROOT "${CMAKE_SOURCE_DIR}/../Aether")
message(STATUS "Aether root: ${AETHER_ROOT}")
add_subdirectory(${AETHER_ROOT} ${CMAKE_BINARY_DIR}/Aether)
add_subdirectory("TinyRogue")
add_subdirectory("Atlas")
add_subdirectory("TerrainEditor")
add_subdirectory("Page")
add_subdirectory("AtlasBuildTool")