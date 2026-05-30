cmake_minimum_required(VERSION 3.24)

project(qui_base
    VERSION 0.1.0
    LANGUAGES C CXX
)

option(QUI_BUILD_DEMO "Build the demonstration executable in source/qui/main.cpp" OFF)

set(QUI_CXX_STANDARD 20 CACHE STRING "C++ standard used to build qui_base")
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
elseif(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)

add_subdirectory(source/glfw EXCLUDE_FROM_ALL)
add_subdirectory(source/ctre EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)

set(QUI_FONT_DIR "${PROJECT_SOURCE_DIR}/source/qui/assets/fonts/geist-mono")

function(qui_apply_optimizations target)
    target_compile_definitions(${target}
        PRIVATE
            $<$<CONFIG:Release>:NDEBUG>
    )

    if(MSVC)
        target_compile_options(${target}
            PRIVATE
                $<$<CONFIG:Release>:/O2>
                $<$<CONFIG:Release>:/Ob2>
                $<$<CONFIG:Release>:/Oi>
                $<$<CONFIG:Release>:/Ot>
                $<$<CONFIG:Release>:/Gy>
                $<$<CONFIG:Release>:/Gw>
        )
        target_link_options(${target}
            PRIVATE
                $<$<CONFIG:Release>:/OPT:REF>
                $<$<CONFIG:Release>:/OPT:ICF>
        )
    else()
        target_compile_options(${target}
            PRIVATE
                $<$<CONFIG:Release>:-O3>
        )
    endif()
endfunction()

file(GLOB QUI_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/source/qui/*.c"
    "${PROJECT_SOURCE_DIR}/source/qui/*.cpp"
)
list(FILTER QUI_SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_library(qui_base STATIC
    ${QUI_SOURCES}
    source/imgui/imgui.cpp
    source/imgui/imgui_draw.cpp
    source/imgui/imgui_tables.cpp
    source/imgui/imgui_widgets.cpp
    source/imgui/misc/cpp/imgui_stdlib.cpp
    source/imgui/backends/imgui_impl_glfw.cpp
    source/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(qui_base::qui_base ALIAS qui_base)

target_compile_definitions(qui_base
    PUBLIC
        NOMINMAX
        QUI_BASE_FONT_DIR="${QUI_FONT_DIR}"
)

target_include_directories(qui_base
    PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/qui/include>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/imgui>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/imgui/backends>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/imgui/misc/cpp>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/stb>"
)

target_link_libraries(qui_base
    PUBLIC
        glfw
        OpenGL::GL
    INTERFACE
        ctre::ctre
)

if(MSVC)
    target_compile_options(qui_base INTERFACE /templateDepth:4096)
endif()

set_target_properties(qui_base PROPERTIES
    OUTPUT_NAME qui_base
    DEBUG_POSTFIX _d
    CXX_STANDARD ${QUI_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

qui_apply_optimizations(qui_base)
qui_apply_optimizations(glfw)

if(QUI_BUILD_DEMO)
    add_executable(qui_demo source/qui/main.cpp)

    target_link_libraries(qui_demo
        PRIVATE
            qui_base::qui_base
    )

    set_target_properties(qui_demo PROPERTIES
        CXX_STANDARD ${QUI_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    qui_apply_optimizations(qui_demo)
endif()
