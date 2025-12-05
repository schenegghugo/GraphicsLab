cmake_minimum_required(VERSION 3.10)
project(GraphicsLab)

set(CMAKE_CXX_STANDARD 17)

# Find Packages
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# Include Directories
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(src)
include_directories(src/libs)

# Source Files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(GraphicsLab ${SOURCES})

# Link Libraries
target_link_libraries(GraphicsLab
    GLEW::GLEW
    glfw
    OpenGL::GL
)


