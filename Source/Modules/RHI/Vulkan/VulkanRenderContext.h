#pragma once

#include "Graphics/RHI/RHI_RenderContext.h"
#include "Graphics/RHI/RHI_Utils.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <mutex>

namespace Graphics {
	struct QueueFamilyIndices {
		u32 GraphicsAndPresentQueueIndex = 0;
		u32 TransferQueueIndex = 0;
	};

	class Vulkan_Image;

	class Vulkan_RenderContext final : public RHI_RenderContext {
	public:
		constexpr static u32 MaxFramesInFlight = 2;

		Vulkan_RenderContext(const RHI_RenderContextDesc& desc);
		virtual ~Vulkan_RenderContext();
		virtual void Shutdown() final override;

		VkResult CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImage* image, VmaAllocation* allocation, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
		void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, VkImageLayout layout, uint32_t width, uint32_t height);
		void CopyBuffer(VkCommandBuffer cmd, size_t size, VkBuffer srcBuffer, VkBuffer dstBuffer);

		virtual void TransitionImageLayout(RHI_CommandBufferHandle cmd, RHI_Image* image, RHI_ImageLayout newLayout) override;
		virtual void WaitDeviceIdle() override;

		virtual void UpdateTextureDescriptors(const std::vector<Memory::Ref<Texture2D>>& textures) override; // TEMP
		virtual u32 AddSampler(uptr sampler) override;

		virtual [[nodiscard]] RHI_ContextHandle GetNativeHandle() const final override { return reinterpret_cast<RHI_ContextHandle>(_instance); }
		virtual [[nodiscard]] RHI_DeviceHandle GetDeviceHandle() const final override { return reinterpret_cast<RHI_DeviceHandle>(_device); }
		virtual [[nodiscard]] RHI_GPUFeatures* GetGPUFeatures() final override { return &_features; };
		[[nodiscard]] VkDevice GetVkDeviceHandle() const { return _device; }
		[[nodiscard]] u32 GetGraphicsAndPresentQueueIndex() const { return _indices.GraphicsAndPresentQueueIndex; }
		[[nodiscard]] u32 GetTransferQueueIndex() const { return _indices.TransferQueueIndex; }
		[[nodiscard]] VkQueue GetGraphicsAndPresentQueue() const { return _graphicsAndPresentQueue; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return _physicalDevice; }
		[[nodiscard]] VkSurfaceKHR GetVkSurface() const { return _surface; }
		[[nodiscard]] VmaAllocator GetAllocator() const { return _allocator; }
		[[nodicsard]] VkAllocationCallbacks GetAllocationCallbakcs() const { return _allocationCallbacks; }
	private:
		void CreateInstance();
		void CreateVMA();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateDevice();
		void CreateDescriptorSets();

		std::vector<const char*> GetRequiredExtensions();
		QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice physicalDevice);
	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		
		RHI_GPUFeatures _features;
		RHI_RenderContextDesc _desc;
		QueueFamilyIndices _indices;
		VkQueue _graphicsAndPresentQueue = VK_NULL_HANDLE;
		VkQueue _transferQueue = VK_NULL_HANDLE;
		std::mutex _queueMutex;

		VmaAllocator _allocator;
		VkAllocationCallbacks _allocationCallbacks;

		VkDescriptorSet _globalDescSet; // TEMP
		std::vector<VkSampler> _samplers;

		std::vector<const char*> _extensions;
		std::vector<const char*> _layers;
#ifdef _DEBUG
		bool _enableValidationLayer = true;
#else 
		bool _enableValidationLayer = false;
#endif
	};
}