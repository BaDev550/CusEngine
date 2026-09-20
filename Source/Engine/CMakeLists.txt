file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Engine SHARED ${SOURCECODE} ${GENERATED_SOURCES})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine 
    PUBLIC 
        glm::glm-header-only
    PRIVATE
        glfw 
        unofficial::shaderc::shaderc
)