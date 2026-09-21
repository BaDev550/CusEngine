file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(Runtime STATIC ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

target_include_directories(Runtime PUBLIC "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(Runtime 
    PUBLIC 
        glm::glm-header-only
    PRIVATE
        glfw 
        imgui::imgui
        Vulkan::Vulkan
	    GPUOpen::VulkanMemoryAllocator
        unofficial::shaderc::shaderc
)