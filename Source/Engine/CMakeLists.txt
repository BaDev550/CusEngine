file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Engine SHARED ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_include_directories(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(Engine 
    PUBLIC 
        glm::glm-header-only
    PRIVATE
        Runtime
        glfw 
        imgui::imgui
        unofficial::shaderc::shaderc
)