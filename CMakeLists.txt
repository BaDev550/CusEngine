cmake_minimum_required(VERSION 3.20)

set(VCPKG_NOTICE_OPTIONS "-quiet" CACHE STRING "" FORCE)
set(VCPKG_VERBOSE OFF CACHE BOOL "" FORCE)

project("CusEngine")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
endforeach()

find_package(Python3 COMPONENTS Interpreter REQUIRED)

macro(cusengine_generate_reflection TARGET_NAME SOURCES_VAR_NAME)

    set(GENERATED_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/Generated"
    )

    file(MAKE_DIRECTORY "${GENERATED_DIR}")

    set(_GENERATED_SOURCES "")

    foreach(SOURCE_FILE IN LISTS ${SOURCES_VAR_NAME})

        if(SOURCE_FILE MATCHES "\\.h$")

            file(READ "${SOURCE_FILE}" FILE_CONTENT)

            string(
                REGEX MATCH
                "CCLASS\\(\\)[ \t\r\n]*class"
                MACRO_FOUND
                "${FILE_CONTENT}"
            )

            if(MACRO_FOUND)

                get_filename_component(
                    BASE_NAME
                    "${SOURCE_FILE}"
                    NAME_WE
                )

                set(
                    GENERATED_CPP
                    "${GENERATED_DIR}/${BASE_NAME}.gen.cpp"
                )

                message(STATUS "Reflection: ${SOURCE_FILE} -> ${GENERATED_CPP}")
                add_custom_command(
                    OUTPUT "${GENERATED_CPP}"
                    COMMAND
                        Python3::Interpreter
                        "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py"
                        "${SOURCE_FILE}"
                        "${GENERATED_CPP}"
                        "${CMAKE_CURRENT_SOURCE_DIR}"

                    DEPENDS
                        "${SOURCE_FILE}"
                        "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py"

                    COMMENT
                        "Generating reflection for ${BASE_NAME}.h"
                    VERBATIM
                )

                list(
                    APPEND
                    _GENERATED_SOURCES
                    "${GENERATED_CPP}"
                )

            endif()

        endif()

    endforeach()

    list(
        APPEND
        ${SOURCES_VAR_NAME}
        ${_GENERATED_SOURCES}
    )

endmacro()

add_subdirectory("Source/Engine")
add_subdirectory("Source/Runtime")
add_subdirectory("Source/Playground")
add_subdirectory("Source/LevelEditor")