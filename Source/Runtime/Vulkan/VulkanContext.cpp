#include "VulkanContext.h"

#include <Runtime/Definitions/Logger.h>
#include <Runtime/Memory/Memory.h>

#include <Runtime/Vulkan/VulkanSwapchain.h>
#include <Runtime/Vulkan/VulkanFence.h>
#include <Runtime/Vulkan/VulkanQueue.h>
#include <Runtime/Vulkan/VulkanCommandPool.h>
#include <Runtime/Vulkan/VulkanBuffer.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Runtime::RHI {
#define ENABLE_FEATURE_IF_SUPPORTED(supported, feature) \
	Logger::Info("rhi_object_vulkan_context", "{}: [{}, {}]", #supported, feature ? "Supported" : "Not supported", _desc.features.supported ? "Enabled" : "Disabled"); \
	if (_desc.features.supported && !feature) { throw std::runtime_error(#supported " is not supported"); } \
	else { feature = _desc.features.supported; }

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		Logger::Error("rhi_object_vulkan_context", "%s", pCallbackData->pMessage);
		return VK_FALSE;
	}

	void* VKAPI_PTR T_AllocationFunction(
		void* pUserData,
		size_t                  size,
		size_t                  alignment,
		VkSystemAllocationScope allocationScope)
	{
		return Mem::Allocator::Allocate(size, alignment);
	}

	void* VKAPI_PTR T_ReallocationFunction(
		void* pUserData,
		void* pOriginal,
		size_t                  size,
		size_t                  alignment,
		VkSystemAllocationScope allocationScope)
	{
		if (!size) {
			Mem::Allocator::Free(pOriginal);
			return nullptr;
		}
		Mem::Allocator::Free(pOriginal);

		return Mem::Allocator::Allocate(size, alignment);
	}

	void VKAPI_PTR T_FreeFunction(
		void* pUserData,
		void* pMemory)
	{
		Mem::Allocator::Free(pMemory);
	}

	Context* CreateContext(const ContextDesc& desc) {
		return Mem::Allocator::Construct<VulkanContext>(desc);
	}

	VulkanContext::VulkanContext(const ContextDesc& desc) : _desc(desc) {
		try {
			CreateInstance();
			CreateSurface();
			PickPhysicalDevice();
			CreateDevice();
			CreateVMA();
			CreateGlobalSampler();
		}
		catch (const std::runtime_error& err) {
			Logger::Assert(false, "rhi_object_vulkan_context", "{}", err.what());
		}
	}

	VulkanContext::~VulkanContext() {

	}

	void VulkanContext::InitializeImGui() {
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(_device, &pool_info, nullptr, &_imguiDescriptorPool); // add check

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForVulkan(_desc.windowHandle, true);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = _instance;
		init_info.ApiVersion = VK_API_VERSION_1_3;
		init_info.PhysicalDevice = _physicalDevice;
		init_info.Device = _device;
		init_info.Queue = _graphicsAndPresentQueue->GetVkQueue();
		init_info.DescriptorPool = _imguiDescriptorPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;

		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		VkPipelineRenderingCreateInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		info.colorAttachmentCount = 1;
		info.pColorAttachmentFormats = &format;
		info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = info;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void VulkanContext::DestroyImGui() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		vkDestroyDescriptorPool(_device, _imguiDescriptorPool, nullptr);
		ImGui::DestroyContext();
	}
	
	void VulkanContext::Shutdown() {
		for (auto& [type, sampler] : _samplers) {
			vkDestroySampler(_device, sampler, nullptr);
		}

		if (_graphicsAndPresentQueue) Mem::Allocator::Destroy(_graphicsAndPresentQueue);
		if (_allocator) vmaDestroyAllocator(_allocator);
		if (_device) vkDestroyDevice(_device, nullptr);
		if (_surface) vkDestroySurfaceKHR(_instance, _surface, nullptr);
		if (_instance) vkDestroyInstance(_instance, nullptr);
	}

	Buffer* VulkanContext::CreateBuffer(const BufferDesc& desc)
	{
		VulkanBuffer* buffer = Mem::Allocator::Construct<VulkanBuffer>(this, desc);

		VkBufferUsageFlags usage = Utils::GetVkBufferUsage(desc.usage);
		VmaMemoryUsage memoryUsage = Utils::GetVkMemoryUsage(desc.memoryUsage);
		VmaAllocationCreateFlags allocFlags = Utils::GetVkAllocationFlags(desc.allocationFlags);

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = desc.size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = memoryUsage;
		allocInfo.flags = allocFlags;
		VmaAllocationInfo info{};
		Logger::Assert((
			vmaCreateBuffer(
				_allocator,
				&bufferInfo,
				&allocInfo,
				&buffer->_buffer,
				&buffer->_allocation,
				&info) == VK_SUCCESS), buffer->GetObjectDebugName(), "Failed to create buffer!");
		
		buffer->_mappedPtr = info.pMappedData;
		buffer->_allocationSize = info.size;

		buffer->SetObjectDebugName("ROV_buffer");
		return buffer;
	}

	Image* VulkanContext::CreateImage(const ImageDesc& desc)
	{
		VulkanImage* image = Mem::Allocator::Construct<VulkanImage>(this, desc);
		auto it = _samplers.find(desc.sampler);
		if (it == _samplers.end()) {
			Logger::Fatal("VulkanContext", "Requested image sampler is not in samplers list");
			return nullptr;
		}

		VkFormat vkFormat = Utils::GetVkFormat(desc.format); // mybe change this
		VkImageTiling vkTiling = Utils::GetVkImageTiling(desc.tileMode);
		VkImageUsageFlags vKUsage = Utils::GetVkImageUsage(desc.usage);
		VkImageAspectFlags vkAspectFlags = Utils::IsFormatDepth(desc.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = vkFormat;
		imageInfo.extent = VkExtent3D(desc.width, desc.height, 1.0f);
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = vkTiling;
		imageInfo.usage = vKUsage;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		vmaCreateImage(_allocator, &imageInfo, &allocInfo, &image->_image, &image->_allocation, nullptr);

		if (desc.view.type != ImageViewType::None) {
			VkImageViewType type = Utils::GetVkImageViewType(desc.view.type);
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image->_image;
			viewInfo.format = vkFormat;
			viewInfo.viewType = type;
			viewInfo.subresourceRange.aspectMask = vkAspectFlags;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.subresourceRange.baseMipLevel = desc.view.mipCount;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			vkCreateImageView(_device, &viewInfo, nullptr, &image->_imageView);
		}

		image->SetObjectDebugName("ROV_image");
		return image;
	}

	Swapchain* VulkanContext::CreateSwapchain(const SwapchainDesc& desc) {
		VulkanSwapchain* vkSwapchain = Mem::Allocator::Construct<VulkanSwapchain>(this, desc);
		vkSwapchain->Recreate(vkSwapchain->GetDesc());
		vkSwapchain->SetObjectDebugName("ROV_swapchain");
		return vkSwapchain;
	}

	CommandPool* VulkanContext::CreateCommandPool(const CommandPoolDesc& desc) {
		VulkanCommandPool* pool = Mem::Allocator::Construct<VulkanCommandPool>(this, desc);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = (desc.usage == CommandPoolUsage::Transient) ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = desc.queueFamilyIndex;
		vkCreateCommandPool(_device, &poolInfo, nullptr, &pool->_commandPool);

		pool->SetObjectDebugName("ROV_command_pool");
		return pool;
	}

	Queue* VulkanContext::CreateQueue(const QueueDesc& desc) {
		VulkanQueue* queue = Mem::Allocator::Construct<VulkanQueue>(this, desc);

		u32 queueFamilyIndex = FindQueueFamilyIndex(_physicalDevice, Utils::GetVkQueueFlags(desc.type));
		queue->_queueFamilyIndex = queueFamilyIndex;
		vkGetDeviceQueue(_device, queueFamilyIndex, 0, &queue->_queue);

		queue->SetObjectDebugName("ROV_queue");
		return queue;
	}

	Fence* VulkanContext::CreateFence(const FenceDesc& desc) {
		VulkanFence* fence = Mem::Allocator::Construct<VulkanFence>(this, desc);

		if (desc.type == SemaphoreType::Timeline) {
			VkSemaphoreTypeCreateInfo tlsemaphoreTypeInfo{};
			tlsemaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			tlsemaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
			tlsemaphoreTypeInfo.initialValue = desc.initialValue;

			VkSemaphoreCreateInfo tlsemaphoreCreateInfo{};
			tlsemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			tlsemaphoreCreateInfo.pNext = &tlsemaphoreTypeInfo;

			Logger::Assert((vkCreateSemaphore(_device, &tlsemaphoreCreateInfo, nullptr, &fence->_semaphore) == VK_SUCCESS), "VulkanContext", "Failed to create timeline semaphore!");
		}
		else if (desc.type == SemaphoreType::Binary) {
			VkSemaphoreCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			
			Logger::Assert((vkCreateSemaphore(_device, &createInfo, nullptr, &fence->_semaphore) == VK_SUCCESS), "VulkanContext", "Failed to create render finished binary semaphore!");
		}

		fence->SetObjectDebugName("ROV_fence");
		return fence;
	}

	ContextDesc* VulkanContext::GetDesc() { return &_desc; }

	void VulkanContext::SetObjectDebugName(VkDebugUtilsObjectNameInfoEXT* info) {
		auto pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(_device, "vkSetDebugUtilsObjectNameEXT");
		if (pfnSetDebugUtilsObjectNameEXT != nullptr) {
			pfnSetDebugUtilsObjectNameEXT(_device, info);
		}
	}

	void VulkanContext::CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, VkImageLayout layout, uint32_t width, uint32_t height) {
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

	void VulkanContext::CopyBuffer(VkCommandBuffer cmd, size_t size, VkBuffer srcBuffer, VkBuffer dstBuffer) {
		VkBufferCopy region{};
		region.size = size;
		vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &region);
	}

	void VulkanContext::WaitDeviceIdle() {
		vkDeviceWaitIdle(_device);
	}

	void VulkanContext::TransitionImageLayout(VkCommandBuffer cmd, Image* image, ImageLayout newLayout) {
		if (image->GetDesc()->layout == newLayout) return;

		VulkanImage* vkImage = static_cast<VulkanImage*>(image);
		VkImageLayout vkOldLayout = Utils::GetVkImageLayout(image->GetDesc()->layout);
		VkImageLayout vkNewLayout = Utils::GetVkImageLayout(newLayout);
		VkCommandBuffer vkCmd = reinterpret_cast<VkCommandBuffer>(cmd);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = vkOldLayout;
		barrier.newLayout = vkNewLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = vkImage->GetImage();
		barrier.subresourceRange.aspectMask = Utils::IsFormatDepth(image->GetFormat()) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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
		vkImage->_desc.layout = newLayout;
	}

	u32 VulkanContext::RegisterBindlessImage(Image* image) {
		u32 id = _bindlessImages.size();
		_bindlessImages.push_back(image);
		return id;
	}

#if 0
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
#endif

	void VulkanContext::CreateInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_3;

		_extensions = GetRequiredExtensions();
		if (_desc.enableValidationLayer) {
			_layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = VulkanDebugCallback;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		if (_desc.enableValidationLayer) { createInfo.pNext = &debugInfo; }
		else { createInfo.pNext = nullptr; }
		createInfo.enabledExtensionCount = static_cast<u32>(_extensions.size());
		createInfo.ppEnabledExtensionNames = _extensions.data();
		createInfo.enabledLayerCount = static_cast<u32>(_layers.size());
		createInfo.ppEnabledLayerNames = _layers.data();
		createInfo.pApplicationInfo = &appInfo;

		if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vulkan istance");
		}
		Logger::Info("rhi_object_vulkan_context", "Vulkan instance created");
	}

	void VulkanContext::CreateVMA() {
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

	void VulkanContext::PickPhysicalDevice() {
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
		Logger::Info("rhi_object_vulkan_context", "Selected GPU: {}", properties.deviceName);
		Logger::Info("rhi_object_vulkan_context", "Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
		Logger::Info("rhi_object_vulkan_context", "Driver version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion));
	}

	void VulkanContext::CreateGlobalSampler() {
		auto makeSampler = [&](VkFilter filter, VkSamplerAddressMode addresMode, VkSamplerMipmapMode mipMode, bool compEnable, bool anisotrpyEnable) {
			VkSampler resultSampler = nullptr;
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = filter;
			samplerInfo.minFilter = filter;
			samplerInfo.addressModeU = addresMode;
			samplerInfo.addressModeV = addresMode;
			samplerInfo.addressModeW = addresMode;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.anisotropyEnable = anisotrpyEnable;
			samplerInfo.maxAnisotropy = 16.0f;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = compEnable;
			samplerInfo.mipmapMode = mipMode;

			Logger::Assert((vkCreateSampler(_device, &samplerInfo, nullptr, &resultSampler) == VK_SUCCESS), "VulkanContext", "Failed to create sampler");
			return resultSampler;
			};

		_samplers[StaticSampler::NearestRepeat] = makeSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_NEAREST, false, false);
		
	}

	void VulkanContext::CreateBindless() {
		VkDescriptorPoolSize size[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 }
		};

		VkDescriptorPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		info.maxSets = 1000;
		info.poolSizeCount = std::size(size);
		info.pPoolSizes = size;
		
		vkCreateDescriptorPool(_device, &info, nullptr, &_bindlessDescriptorPool);

		Logger::Info("rhi_object_vulkan_context", "Bindless descriptor pool is created");
	}

	void VulkanContext::CreateDevice() {
		_graphicsAndPresentQueue = Mem::Allocator::Construct<VulkanQueue>(this, QueueDesc{ QueueType::Graphics });
		_graphicsAndPresentQueue->_queueFamilyIndex = FindGraphicsAndPresentQueueIndex(_physicalDevice);
		float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(1);
		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = _graphicsAndPresentQueue->_queueFamilyIndex;
		queueCreateInfos[0].queueCount = 1;
		queueCreateInfos[0].pQueuePriorities = &queuePriority;

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

		ENABLE_FEATURE_IF_SUPPORTED(dynamicRendering, supportedFeatures13.dynamicRendering);
		ENABLE_FEATURE_IF_SUPPORTED(synchronization2, supportedFeatures13.synchronization2);
		ENABLE_FEATURE_IF_SUPPORTED(timelineSemaphore, supportedFeatures12.timelineSemaphore);
		ENABLE_FEATURE_IF_SUPPORTED(bufferDeviceAddress, supportedFeatures12.bufferDeviceAddress);
		ENABLE_FEATURE_IF_SUPPORTED(descriptorIndexing, supportedFeatures12.descriptorIndexing);
		ENABLE_FEATURE_IF_SUPPORTED(runtimeDescriptorArray, supportedFeatures12.runtimeDescriptorArray);
		ENABLE_FEATURE_IF_SUPPORTED(robustBufferAccess, supportedFeatures.features.robustBufferAccess);
		ENABLE_FEATURE_IF_SUPPORTED(samplerAnisotropy, supportedFeatures.features.samplerAnisotropy);

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

		vkGetDeviceQueue(_device, _graphicsAndPresentQueue->GetQueueFamilyIndex(), 0, &_graphicsAndPresentQueue->_queue);
		
		Logger::Info("rhi_object_vulkan_context", "Logical device created");
	}

	void VulkanContext::CreateSurface() {
		glfwCreateWindowSurface(_instance, _desc.windowHandle, nullptr, &_surface);
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (_desc.enableValidationLayer) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	VkPhysicalDeviceFeatures VulkanContext::GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice) {
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		return features;
	}

	VkPhysicalDeviceLimits VulkanContext::GetPhysicalDeviceLimits(VkPhysicalDevice physicalDevice) {
		VkPhysicalDeviceProperties prop{};
		vkGetPhysicalDeviceProperties(physicalDevice, &prop);
		return prop.limits;
	}

	u32 VulkanContext::FindGraphicsAndPresentQueueIndex(VkPhysicalDevice physicalDevice) {
		return FindQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
	}

	u32 VulkanContext::FindQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags queueFlags) {
		u32 queueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueCount, nullptr);
		std::vector<VkQueueFamilyProperties2> queueFamilies(queueCount, { .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueCount, queueFamilies.data());

		for (u32 i = 0; i < queueFamilies.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport);

			const auto& qProps = queueFamilies[i];
			if ((qProps.queueFamilyProperties.queueFlags & queueFlags) && presentSupport) {
				return i;
			}
		}
		return 0;
	}
}