if(MSVC)
    include("${CMAKE_SOURCE_DIR}/CMake/VsConfig.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
file(GLOB_RECURSE FILES
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(MODULE_NAME "TinyRogue")
add_executable(${MODULE_NAME} ${FILES})
target_include_directories(${MODULE_NAME} PUBLIC "${AETHER_ROOT}/Vendor")
target_link_libraries(${MODULE_NAME} Window)
target_link_libraries(${MODULE_NAME} UI)
target_link_libraries(${MODULE_NAME} Render)
target_link_libraries(${MODULE_NAME} IO)
target_link_libraries(${MODULE_NAME} Entry)
target_link_libraries(${MODULE_NAME} miniaudio)