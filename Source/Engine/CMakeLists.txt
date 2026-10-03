file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Engine SHARED ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(nlohmann_json REQUIRED)

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
        nlohmann_json::nlohmann_json
        unofficial::shaderc::shaderc
        ${NVTT_LIBS}
)