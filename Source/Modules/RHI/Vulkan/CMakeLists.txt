file(GLOB_RECURSE VULKAN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(VulkanBackend SHARED ${VULKAN_SOURCES})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanBackend PROPERTY CXX_STANDARD 23)
  set_property(TARGET VulkanBackend PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

target_compile_definitions(VulkanBackend PRIVATE 
	ENGINE_EXPORTS
)

target_include_directories(VulkanBackend PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
	
	PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Game/Source>
)

target_link_libraries(VulkanBackend PRIVATE
	glfw
	glm::glm-header-only
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
)