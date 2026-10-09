cmake_minimum_required(VERSION 3.24)

set(VCPKG_NOTICE_OPTIONS "-quiet" CACHE STRING "" FORCE)
set(VCPKG_VERBOSE OFF CACHE BOOL "" FORCE)

project("CusEngine")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REFLECTION_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/Intermative")

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
endforeach()

add_custom_target(ReflectGen
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py
            --root ${CMAKE_SOURCE_DIR} --project Engine --source Source/Engine
    COMMENT "Generating reflection")

add_subdirectory("Source/Engine")
add_subdirectory("Source/Runtime")
add_subdirectory("Source/Playground")
add_subdirectory("Source/LevelEditor")