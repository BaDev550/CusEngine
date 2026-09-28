#pragma once
#include <Runtime/RHI/Image/RHIImageLayout.h>
#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Common/RHIFormat.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <mutex>

namespace CusEngine::RHI {
	class VulkanContext final : public Context {
	public:
		constexpr static u32 MaxFramesInFlight = 2;

		VulkanContext(const ContextDesc& desc);
		virtual ~VulkanContext();

		virtual void InitializeImGui() override;
		virtual void NewFrameImGui() override;
		virtual void DestroyImGui() override;

		virtual void WaitDeviceIdle() override;
		virtual void Shutdown() final override;

		virtual Commands* CreateCommands(const CommandsDesc& desc) override;
		virtual Buffer* CreateBuffer(const BufferDesc& desc) override;
		virtual Image* CreateImage(const ImageDesc& desc) override;
		virtual Swapchain* CreateSwapchain(const SwapchainDesc& desc) override;
		virtual ContextDesc* GetDesc() override;

		void SetObjectDebugName(VkDebugUtilsObjectNameInfoEXT* info);
		void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, VkImageLayout layout, uint32_t width, uint32_t height); // TODO(0x): move this into commands
		void CopyBuffer(VkCommandBuffer cmd, size_t size, VkBuffer srcBuffer, VkBuffer dstBuffer);
		void TransitionImageLayout(VkCommandBuffer  cmd, Image* image, ImageLayout newLayout);
		u32 RegisterBindlessImage(Image* image);

		[[nodiscard]] VkInstance GetInstance() const { return _instance; }
		[[nodiscard]] VkDevice GetDevice() const { return _device; }
		[[nodiscard]] u32 GetGraphicsAndPresentQueueIndex() const { return _graphicsAndPresentQueueIndex; }
		[[nodiscard]] VkQueue GetGraphicsAndPresentQueue() const { return _graphicsAndPresentQueue; }
		[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
		[[nodiscard]] VkSurfaceKHR GetSurface() const { return _surface; }
		[[nodiscard]] VmaAllocator GetAllocator() const { return _allocator; }
		[[nodicsard]] VkAllocationCallbacks GetAllocationCallbakcs() const { return _allocationCallbacks; }
	protected:
		void CreateInstance();
		void CreateVMA();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateDevice();

		std::vector<const char*> GetRequiredExtensions();
		u32 FindGraphicsAndPresentQueueIndex(VkPhysicalDevice physicalDevice);
	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;

		ContextDesc _desc;
		
		VkQueue _graphicsAndPresentQueue = VK_NULL_HANDLE;
		u32 _graphicsAndPresentQueueIndex = u32_max;

		VmaAllocator _allocator;
		VkAllocationCallbacks _allocationCallbacks;

		VkDescriptorPool _imguiPool;

		std::vector<Image*> _bindlessImages;
		//std::vector<Buffer*> _bindlessBuffers; later

		std::vector<const char*> _extensions;
		std::vector<const char*> _layers;
	};
}