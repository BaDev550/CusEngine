cmake_minimum_required(VERSION 3.24)

project("Tourqe")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REFLECTION_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/Intermative")
set(REFLECTION_TOOL         "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py")

find_package(Python3 REQUIRED COMPONENTS Interpreter)
if(NOT Python3_EXECUTABLE)
    message(FATAL_ERROR "Python3 interpreter not found, needed for some Tools")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(Output)
include(Reflection)

add_subdirectory("Source/Engine")
add_subdirectory("Source/Runtime")
add_subdirectory("Source/Playground")
add_subdirectory("Source/LevelEditor")