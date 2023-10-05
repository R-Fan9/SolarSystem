cmake_minimum_required(VERSION 3.26)
project(SolarSystem)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp glad.c)

include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(SolarSystem ${SOURCE_FILES})

target_link_libraries(SolarSystem glfw3)
