file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 

set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated")
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(GENERATED_SOURCES "")

foreach(SOURCE_FILE ${SOURCECODE})
    if("${SOURCE_FILE}" MATCHES "\\.h$")
        file(READ ${SOURCE_FILE} FILE_CONTENT)
        string(REGEX MATCH "CUS_CLASS\\(\\)[ \t\r\n]*class" MACRO_FOUND "${FILE_CONTENT}")

        if(MACRO_FOUND)
            get_filename_component(BASE_NAME ${SOURCE_FILE} NAME_WE)
            set(GENERATED_CPP "${GENERATED_DIR}/${BASE_NAME}.gen.cpp")

            add_custom_command(
                OUTPUT ${GENERATED_CPP}
                COMMAND python "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py" "${SOURCE_FILE}" "${GENERATED_CPP}" "${CMAKE_CURRENT_SOURCE_DIR}"
                DEPENDS ${SOURCE_FILE} "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py"
                COMMENT "Generating reflection for ${BASE_NAME}.h (Engine)"
            )

            list(APPEND GENERATED_SOURCES ${GENERATED_CPP})
        endif()
    endif()
endforeach()

add_library(Engine SHARED ${SOURCECODE} ${GENERATED_SOURCES})

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(stb REQUIRED)

find_path(NVTT_INCLUDE_DIR NAMES nvtt/nvtt.h REQUIRED)
find_path(MIO_INCLUDE_DIR NAMES mio/mmap.hpp REQUIRED)

find_library(NVTT_REL     NAMES nvtt     PATH_SUFFIXES static REQUIRED)
find_library(NVTHREAD_REL NAMES nvthread PATH_SUFFIXES static)
find_library(NVSQUISH_REL NAMES nvsquish PATH_SUFFIXES static)
find_library(NVMATH_REL   NAMES nvmath   PATH_SUFFIXES static)
find_library(NVIMAGE_REL  NAMES nvimage  PATH_SUFFIXES static)
find_library(NVCORE_REL   NAMES nvcore   PATH_SUFFIXES static)
find_library(BC7_REL      NAMES bc7      PATH_SUFFIXES static)
find_library(BC6H_REL     NAMES bc6h     PATH_SUFFIXES static)

find_library(NVTT_DBG     NAMES nvtt_d     PATH_SUFFIXES static REQUIRED)
find_library(NVTHREAD_DBG NAMES nvthread_d PATH_SUFFIXES static)
find_library(NVSQUISH_DBG NAMES nvsquish_d PATH_SUFFIXES static)
find_library(NVMATH_DBG   NAMES nvmath_d   PATH_SUFFIXES static)
find_library(NVIMAGE_DBG  NAMES nvimage_d  PATH_SUFFIXES static)
find_library(NVCORE_DBG   NAMES nvcore_d   PATH_SUFFIXES static)
find_library(BC7_DBG      NAMES bc7_d      PATH_SUFFIXES static)
find_library(BC6H_DBG     NAMES bc6h_d     PATH_SUFFIXES static)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(NVTT_LIBS
        ${NVTT_DBG} ${NVTHREAD_DBG} ${NVSQUISH_DBG} ${NVMATH_DBG}
        ${NVIMAGE_DBG} ${NVCORE_DBG} ${BC7_DBG} ${BC6H_DBG}
    )
else()
    set(NVTT_LIBS
        ${NVTT_REL} ${NVTHREAD_REL} ${NVSQUISH_REL} ${NVMATH_REL}
        ${NVIMAGE_REL} ${NVCORE_REL} ${BC7_REL} ${BC6H_REL}
    )
endif()

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_include_directories(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Source" PRIVATE ${MIO_INCLUDE_DIR} ${NVTT_INCLUDE_DIR} ${Stb_INCLUDE_DIR})

target_link_libraries(Engine 
    PUBLIC 
        glm::glm-header-only
    PRIVATE
        Runtime
        glfw 
        imgui::imgui
        unofficial::shaderc::shaderc
        ${NVTT_LIBS}
)