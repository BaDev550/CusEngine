file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(RHI_Vulkan STATIC ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

target_compile_definitions(RHI_Vulkan PRIVATE ENGINE_EXPORTS_STATIC_LIB)

target_link_libraries(RHI_Vulkan PRIVATE
	Engine
	glfw
	glm::glm-header-only
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
)