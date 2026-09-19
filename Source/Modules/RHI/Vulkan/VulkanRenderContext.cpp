#include "VulkanRenderContext.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"

#include "Core/Logger.h"
#include "Graphics/Texture.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Graphics {
#define ENABLE_FEATURE_IF_SUPPORTED(supported, feature) \
	Logger::Info("Vulkan Render Context", "{}: {}", #supported, feature ? "Supported" : "Not supported"); \
	if (_features.supported && !feature) { throw std::runtime_error(#supported " is not supported"); } \
	else { feature = _features.supported; }

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		Logger::Error("Vulkan Render Context", "%s", pCallbackData->pMessage);
		return VK_FALSE;
	}

	void* VKAPI_PTR T_AllocationFunction(
		void* pUserData,
		size_t                  size,
		size_t                  alignment,
		VkSystemAllocationScope allocationScope)
	{
		return Memory::Allocator::Allocate(size, alignment);
	}

	void* VKAPI_PTR T_ReallocationFunction(
		void* pUserData,
		void* pOriginal,
		size_t                  size,
		size_t                  alignment,
		VkSystemAllocationScope allocationScope)
	{
		if (!size) {
			Memory::Allocator::Free(pOriginal);
			return nullptr;
		}
		Memory::Allocator::Free(pOriginal);

		return Memory::Allocator::Allocate(size, alignment);
	}

	void VKAPI_PTR T_FreeFunction(
		void* pUserData,
		void* pMemory)
	{

	}

	Vulkan_RenderContext::Vulkan_RenderContext(const RHI_RenderContextDesc& desc) : _desc(desc) {
		try {
			CreateInstance();
			CreateSurface();
			PickPhysicalDevice();
			CreateDevice();
			CreateVMA();
		}
		catch (const std::runtime_error& err) {
			Logger::Assert(false, "Vulkan Render Context", "{}", err.what());
		}
	}

	Vulkan_RenderContext::~Vulkan_RenderContext() { }

	void Vulkan_RenderContext::Shutdown() {
		if (_allocator) vmaDestroyAllocator(_allocator);

		if (_device) vkDestroyDevice(_device, nullptr);

		if (_surface) vkDestroySurfaceKHR(_instance, _surface, nullptr);
		if (_instance) vkDestroyInstance(_instance, nullptr);
	}

	VkResult Vulkan_RenderContext::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImage* image, VmaAllocation* allocation, VkImageTiling tiling) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = format;
		imageInfo.extent = VkExtent3D(width, height, 1.0f);
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = tiling;
		imageInfo.usage = usage;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		return vmaCreateImage(_allocator, &imageInfo, &allocInfo, image, allocation, nullptr);
	}

	void Vulkan_RenderContext::CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, VkImageLayout layout, uint32_t width, uint32_t height) {
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageOffset = VkOffset3D(0.0f, 0.0f, 0.0f);
		region.imageExtent = VkExtent3D(width, height, 1.0f);
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.layerCount = 1;
		vkCmdCopyBufferToImage(cmd, buffer, image, layout, 1, &region);
	}

	void Vulkan_RenderContext::CopyBuffer(VkCommandBuffer cmd, size_t size, VkBuffer srcBuffer, VkBuffer dstBuffer) {
		VkBufferCopy region{};
		region.size = size;
		vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &region);
	}

	void Vulkan_RenderContext::TransitionImageLayout(RHI_CommandBufferHandle cmd, RHI_Image* image, RHI_ImageLayout newLayout) {
		if (image->GetDesc()->Layout == newLayout) return;

		Vulkan_Image* vkImage = static_cast<Vulkan_Image*>(image);
		VkImageLayout vkOldLayout = Utils::RHI_GetVkImageLayout(image->GetDesc()->Layout);
		VkImageLayout vkNewLayout = Utils::RHI_GetVkImageLayout(newLayout);
		VkCommandBuffer vkCmd = reinterpret_cast<VkCommandBuffer>(cmd);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = vkOldLayout;
		barrier.newLayout = vkNewLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = vkImage->GetVkImage();
		barrier.subresourceRange.aspectMask = Utils::RHI_IsFormatDepth(image->GetFormat()) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;
		if (vkOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkNewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && vkNewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = 0;
			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_NONE;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && vkNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (vkOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else {
			Logger::Error("Vulkan Render Context", "Layout transmition is not supported!");
			return;
		}
		vkCmdPipelineBarrier(vkCmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		vkImage->_desc.Layout = newLayout;
	}

	void Vulkan_RenderContext::WaitDeviceIdle() {
		vkDeviceWaitIdle(_device);
	}

	void Vulkan_RenderContext::UpdateTextureDescriptors(const std::vector<Memory::Ref<Texture2D>>& textures) {
		std::vector<VkDescriptorImageInfo> imageDescriptors;
		imageDescriptors.reserve(textures.size());
		for (const auto& texture : textures) {
			Vulkan_Image* vkImage = static_cast<Vulkan_Image*>(texture->GetImage().Get());

			VkDescriptorImageInfo info{};
			info.imageView = vkImage->GetVkImageView();
			info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			info.sampler = reinterpret_cast<VkSampler>(_samplers[vkImage->GetSamplerId()]);
			imageDescriptors.push_back(info);
		}
		VkWriteDescriptorSet descSetWrite{};
		descSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descSetWrite.dstSet = _globalDescSet;
		descSetWrite.dstBinding = 0;
		descSetWrite.dstArrayElement = 0;
		descSetWrite.descriptorCount = static_cast<u32>(imageDescriptors.size());
		descSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSetWrite.pImageInfo = imageDescriptors.data();

		vkUpdateDescriptorSets(_device, 1, &descSetWrite, 0, nullptr);
	}

	u32 Vulkan_RenderContext::AddSampler(uptr sampler) {
		_samplers.push_back(reinterpret_cast<VkSampler>(sampler));
		return _samplers.size();
	}

	void Vulkan_RenderContext::CreateInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_3;

		_extensions = GetRequiredExtensions();
		if (_enableValidationLayer) {
			_layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = VulkanDebugCallback;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		if (_enableValidationLayer) { createInfo.pNext = &debugInfo; }
		else { createInfo.pNext = nullptr; }
		createInfo.enabledExtensionCount = static_cast<u32>(_extensions.size());
		createInfo.ppEnabledExtensionNames = _extensions.data();
		createInfo.enabledLayerCount = static_cast<u32>(_layers.size());
		createInfo.ppEnabledLayerNames = _layers.data();
		createInfo.pApplicationInfo = &appInfo;

		if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vulkan istance");
		}
		Logger::Info("Vulkan Render Context", "Vulkan instance created");
	}

	void Vulkan_RenderContext::CreateVMA() {
		VmaVulkanFunctions funcs{};
		funcs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		funcs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
		
		_allocationCallbacks.pUserData = nullptr;
		_allocationCallbacks.pfnAllocation = T_AllocationFunction;
		_allocationCallbacks.pfnReallocation = T_ReallocationFunction;
		_allocationCallbacks.pfnFree = T_FreeFunction;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.instance = _instance;
		createInfo.device = _device;
		createInfo.physicalDevice = _physicalDevice;
		createInfo.pAllocationCallbacks = &_allocationCallbacks;
		createInfo.pVulkanFunctions = &funcs;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		vmaCreateAllocator(&createInfo, &_allocator);
	}

	void Vulkan_RenderContext::PickPhysicalDevice() {
		u32 physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data());

		if (physicalDeviceCount == 0) {
			throw std::runtime_error("Failed to find GPUs with vulkan support");
		}

		_physicalDevice = physicalDevices[0];
		for (const auto& pd : physicalDevices) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(pd, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				_physicalDevice = pd;
				break;
			}
		}

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
		Logger::Info("Vulkan Render Context", "Selected GPU: {}", properties.deviceName);
		Logger::Info("Vulkan Render Context", "	Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
		Logger::Info("Vulkan Render Context", "	Driver version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion));
	}

	void Vulkan_RenderContext::CreateDevice() {
		_features.DynamicRendering = true;
		_features.Synchronization2 = true;
		_features.DescriptorIndexing = true;
		_features.BufferDeviceAddress = true;
		_features.RobustBufferAccess = true;
		_features.SamplerAnisotropy = true;
		_features.TimelineSemaphore = true;
		_features.RuntimeDescriptorArray = true;

		_indices.GraphicsAndPresentQueueIndex = FindQueueFamilyIndices(_physicalDevice).GraphicsAndPresentQueueIndex;
		_indices.TransferQueueIndex = FindQueueFamilyIndices(_physicalDevice).TransferQueueIndex;
		float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(2);
		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = _indices.GraphicsAndPresentQueueIndex;
		queueCreateInfos[0].queueCount = 1;
		queueCreateInfos[0].pQueuePriorities = &queuePriority;

		queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[1].queueFamilyIndex = _indices.TransferQueueIndex;
		queueCreateInfos[1].queueCount = 1;
		queueCreateInfos[1].pQueuePriorities = &queuePriority;

		VkPhysicalDeviceVulkan13Features supportedFeatures13{};
		supportedFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		supportedFeatures13.pNext = nullptr;
		VkPhysicalDeviceVulkan12Features supportedFeatures12{};
		supportedFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		supportedFeatures12.pNext = &supportedFeatures13;

		VkPhysicalDeviceFeatures2 supportedFeatures{};
		supportedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		supportedFeatures.pNext = &supportedFeatures12;
		vkGetPhysicalDeviceFeatures2(_physicalDevice, &supportedFeatures);

		ENABLE_FEATURE_IF_SUPPORTED(DynamicRendering, supportedFeatures13.dynamicRendering);
		ENABLE_FEATURE_IF_SUPPORTED(Synchronization2, supportedFeatures13.synchronization2);
		ENABLE_FEATURE_IF_SUPPORTED(TimelineSemaphore, supportedFeatures12.timelineSemaphore);
		ENABLE_FEATURE_IF_SUPPORTED(BufferDeviceAddress, supportedFeatures12.bufferDeviceAddress);
		ENABLE_FEATURE_IF_SUPPORTED(DescriptorIndexing, supportedFeatures12.descriptorIndexing);
		ENABLE_FEATURE_IF_SUPPORTED(RuntimeDescriptorArray, supportedFeatures12.runtimeDescriptorArray);
		ENABLE_FEATURE_IF_SUPPORTED(RobustBufferAccess, supportedFeatures.features.robustBufferAccess);
		ENABLE_FEATURE_IF_SUPPORTED(SamplerAnisotropy, supportedFeatures.features.samplerAnisotropy);

		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = &supportedFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device");
		}

		vkGetDeviceQueue(_device, _indices.GraphicsAndPresentQueueIndex, 0, &_graphicsAndPresentQueue);
		vkGetDeviceQueue(_device, _indices.TransferQueueIndex, 0, &_transferQueue);

		Logger::Info("Vulkan Render Context", "Logical device created");
	}

	void Vulkan_RenderContext::CreateDescriptorSets() {
		std::array<VkDescriptorPoolSize, 1> poolSizes{
			VkDescriptorPoolSize{}
		}
	}

	void Vulkan_RenderContext::CreateSurface() {
		glfwCreateWindowSurface(_instance, _desc.WindowHandle, nullptr, &_surface);
	}

	std::vector<const char*> Vulkan_RenderContext::GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (_enableValidationLayer) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	QueueFamilyIndices Vulkan_RenderContext::FindQueueFamilyIndices(VkPhysicalDevice physicalDevice) {
		uint32_t queueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueCount, nullptr);
		std::vector<VkQueueFamilyProperties2> queueFamilies(queueCount, { .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueCount, queueFamilies.data());

		QueueFamilyIndices indices;

		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport);

			const auto& qProps = queueFamilies[i];
			if ((qProps.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && presentSupport) {
				indices.GraphicsAndPresentQueueIndex = i;
			}
			else if (qProps.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				indices.TransferQueueIndex = i;
			}
		}
		return indices;
	}
}