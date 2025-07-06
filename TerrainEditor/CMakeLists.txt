if(MSVC)
    include("${CMAKE_SOURCE_DIR}/CMake/VsConfig.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
file(GLOB_RECURSE FILES
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(MODULE_NAME "TerrainEditor")
add_executable(${MODULE_NAME}  ${FILES})
target_link_libraries(${MODULE_NAME} PUBLIC Atlas)
target_link_libraries(${MODULE_NAME} PUBLIC Page)
target_link_libraries(${MODULE_NAME} PUBLIC Entry)
target_link_libraries(${MODULE_NAME} PUBLIC Audio)
