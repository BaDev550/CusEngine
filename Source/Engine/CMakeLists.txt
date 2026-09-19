file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Engine ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine 
    PUBLIC 
        RHI_Vulkan
        glm::glm-header-only
    PRIVATE
        glfw 
        unofficial::shaderc::shaderc
)