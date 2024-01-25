cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 11)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )



add_executable(project main.cpp utils.h utils.cpp)

target_link_libraries(project GL GLU GLEW glut glfw)
set(CMAKE_BUILD_TYPE Debug)
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
