if(MSVC)
    include("${CMAKE_SOURCE_DIR}/CMake/VsConfig.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
file(GLOB_RECURSE FILES
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(MODULE_NAME "Page")
add_library(${MODULE_NAME} STATIC ${FILES})
target_include_directories(${MODULE_NAME} PUBLIC "Source")
target_link_libraries(${MODULE_NAME} PUBLIC Filesystem)
target_link_libraries(${MODULE_NAME} PUBLIC Core)
target_link_libraries(${MODULE_NAME} PUBLIC Render)
target_link_libraries(${MODULE_NAME} PUBLIC Resource)
target_link_libraries(${MODULE_NAME} PUBLIC UI)
