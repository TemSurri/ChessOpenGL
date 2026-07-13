cmake_minimum_required(VERSION 3.20)

project(Chess LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw/glfw-3.4)

add_library(glad
    external/glad/src/glad.c
 "gui.h")

target_include_directories(glad PUBLIC
    external/glad/include
)

add_executable(Chess
    main.cpp
    game.cpp
    minimax.cpp
    gui.cpp
 "gui.h")

target_link_libraries(Chess PRIVATE
    glad
    glfw
    opengl32
)