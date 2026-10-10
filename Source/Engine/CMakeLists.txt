file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Engine SHARED ${SOURCECODE})
add_reflection(Engine)

include(NVTT)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(nlohmann_json REQUIRED)

find_path(MIO_INCLUDE_DIR NAMES mio/mmap.hpp REQUIRED)

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_include_directories(Engine 
    PUBLIC 
        "${CMAKE_SOURCE_DIR}/Source" 
    PRIVATE 
        ${REFLECTION_INCLUDE_PATH} 
        ${MIO_INCLUDE_DIR} 
        ${NVTT_INCLUDE_DIR} 
        ${Stb_INCLUDE_DIR}
)

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