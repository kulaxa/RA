cmake_minimum_required(VERSION 3.10)
project(RA_lab2)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

## Find DevIL package
#find_package(IL REQUIRED)

# Find DevIL library manually
find_path(IL_INCLUDE_DIR IL/il.h)
find_library(IL_LIBRARIES NAMES IL ILU ILUT)

# Add ImGui source files
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui_tables.cpp
        imgui/imgui_internal.h
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glut.h
        imgui/backends/imgui_impl_glut.cpp
        imgui/backends/imgui_impl_opengl2.h
        imgui/backends/imgui_impl_opengl2.cpp


)

# Include directories for ImGui
include_directories(imgui)

if (IL_INCLUDE_DIR AND IL_LIBRARIES)
    message(STATUS "DevIL found: ${IL_INCLUDE_DIR}")
    include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${IL_INCLUDE_DIR})
    add_executable(project main.cpp utils.h utils.cpp
            particle.cpp
            particle.h
            particle_system.cpp
            particle_system.h
            ${IMGUI_SOURCES}
    )

    target_link_libraries(project GL GLU GLEW glut glfw ${IL_LIBRARIES})
    set(CMAKE_BUILD_TYPE Debug)
    file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
else()
    message(FATAL_ERROR "DevIL not found. Please install DevIL and try again.")
endif()





