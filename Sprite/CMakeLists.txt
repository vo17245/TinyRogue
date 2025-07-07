if(MSVC)
    include("${CMAKE_SOURCE_DIR}/CMake/VsConfig.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
file(GLOB_RECURSE FILES
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(MODULE_NAME "Sprite")
add_library(${MODULE_NAME} STATIC ${FILES})
target_include_directories(${MODULE_NAME} PUBLIC
    "Source"
)
target_link_libraries(${MODULE_NAME} PUBLIC
Core 
Render 
Resource 
)