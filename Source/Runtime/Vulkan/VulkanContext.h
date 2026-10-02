#pragma once
#include <Runtime/RHI/Image/RHIImageLayout.h>
#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Common/RHIFormat.h>
#include <Runtime/RHI/Image/RHIStaticSampler.h>
#include <Runtime/Vulkan/VulkanQueue.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <array>

namespace Runtime::RHI {
	class VulkanContext final : public Context {
	public:
		VulkanContext(const ContextDesc& desc);
		virtual ~VulkanContext();

		virtual void InitializeImGui() override;
		virtual void DestroyImGui() override;

		virtual void WaitDeviceIdle() override;
		virtual void Shutdown() final override;

		virtual Buffer* CreateBuffer(const BufferDesc& desc) override;
		virtual Image* CreateImage(const ImageDesc& desc) override;
		virtual Swapchain* CreateSwapchain(const SwapchainDesc& desc) override;
		virtual CommandPool* CreateCommandPool(const CommandPoolDesc& desc) override;
		virtual Queue* CreateQueue(const QueueDesc& desc) override;
		virtual Fence* CreateFence(const FenceDesc& desc) override;
		virtual Pipeline* CreatePipeline(const PipelineDesc& desc) override;
		virtual Queue* GetGraphicsQueue() { return _graphicsAndPresentQueue; }
		virtual ContextDesc* GetDesc() override;

		void SetObjectDebugName(VkDebugUtilsObjectNameInfoEXT* info);
		void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, VkImageLayout layout, uint32_t width, uint32_t height); // TODO(0x): move this into commands
		void CopyBuffer(VkCommandBuffer cmd, size_t size, VkBuffer srcBuffer, VkBuffer dstBuffer);
		void TransitionImageLayout(VkCommandBuffer cmd, Image* image, ImageLayout newLayout);
		u32 RegisterBindlessImage(Image* image);
		u32 GetSamplerId(StaticSampler sampler);

		[[nodiscard]] VkInstance GetInstance() const { return _instance; }
		[[nodiscard]] VkDevice GetDevice() const { return _device; }
		[[nodiscard]] u32 GetGraphicsAndPresentQueueIndex() const { return _graphicsAndPresentQueue->GetQueueFamilyIndex(); }
		[[nodiscard]] VkQueue GetGraphicsAndPresentQueue() const { return _graphicsAndPresentQueue->GetVkQueue(); }
		[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
		[[nodiscard]] VkSurfaceKHR GetSurface() const { return _surface; }
		[[nodiscard]] VmaAllocator GetAllocator() const { return _allocator; }
		[[nodiscard]] VkAllocationCallbacks GetAllocationCallbacks() const { return _allocationCallbacks; }
		[[nodiscard]] VkDescriptorSet GetBindlessDescriptorSet() const { return _bindlessDescriptorSet; }
		[[nodiscard]] VkDescriptorSet GetGlobalSamplerSet() const { return _globalSamplerSet; }
		[[nodiscard]] VkSampler GetSampler(StaticSampler sampler);
	protected:
		void CreateInstance();
		void CreateVMA();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateSamplers();
		void CreateGlobalDescriptors();
		void CreateBindless();
		void CreateDevice();

		std::vector<const char*> GetRequiredExtensions();
		VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice);
		VkPhysicalDeviceLimits GetPhysicalDeviceLimits(VkPhysicalDevice physicalDevice);
		u32 FindGraphicsAndPresentQueueIndex(VkPhysicalDevice physicalDevice);
		u32 FindQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags queueFlags);
	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		ContextDesc _desc;
		
		VulkanQueue* _graphicsAndPresentQueue = nullptr;
		
		VmaAllocator _allocator;
		VkAllocationCallbacks _allocationCallbacks;

		VkDescriptorPool _imguiDescriptorPool;

		VkDescriptorSetLayout _bindlessDescriptorLayout;
		VkDescriptorPool _bindlessDescriptorPool;
		VkDescriptorSet _bindlessDescriptorSet;

		VkDescriptorSetLayout _globalSamplerLayout;
		VkDescriptorSet _globalSamplerSet;

		std::vector<Image*> _bindlessImages;
		std::vector<Buffer*> _bindlessBuffers;
		std::array<VkSampler, (usize)StaticSampler::COUNT> _samplers;

		std::vector<const char*> _extensions;
		std::vector<const char*> _layers;
	};
}