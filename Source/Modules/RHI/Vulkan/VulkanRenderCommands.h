#pragma once

#include "Graphics/RHI/RHI_RenderCommands.h"

#include "VulkanRenderContext.h"
#include "VulkanSwapchain.h"

namespace Graphics {
	class Vulkan_RenderCommands : public RHI_RenderCommands {
	public:
		struct FrameData {
			VkCommandPool CommandPool = VK_NULL_HANDLE;
			VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
			VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
			std::vector<Memory::Ref<RHI_Object>> TrackedObjects;
		};

		Vulkan_RenderCommands(Vulkan_RenderContext* context, Vulkan_Swapchain* swapchain);
		virtual ~Vulkan_RenderCommands();

		virtual void BeginFrame() override final;
		virtual void EndFrame() override final;
		virtual void Submit(RHI_CommandFunc func) override final;
		virtual void Track(const Memory::Ref<RHI_Object>& object) override final;
		virtual void Wait() override final;

		virtual void BeginDynamicRendering(std::vector<Memory::Ref<RHI_Image>> colorAttachments, Memory::Ref<RHI_Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) override final;
		virtual void EndDynamicRendering() override final;

		virtual void TransitionImageLayout(RHI_Image* image, RHI_ImageLayout newLayout) override;
		virtual void CopyBuffer(RHI_Buffer* srcBuffer, RHI_Buffer* dstBuffer, size_t size) override;
		virtual void CopyBufferToImage(RHI_Buffer* buffer, RHI_Image* image, RHI_ImageLayout layout, u32 width, u32 height) override;
		virtual u32 RegisterBindlessImage(const Memory::Ref<RHI_Image>& image) override;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept override final;
		virtual [[nodiscard]] RHI_Swapchain* GetTargetSwapchain() const override final;
		virtual [[nodiscard]] RHI_RenderContext* GetTargetRenderContext() const override final;

		[[nodiscard]] Vulkan_RenderContext* GetVkContext() const { return _context; }
	private:
		[[nodiscard]] FrameData* GetCurrentFrameData();

		Vulkan_RenderContext* _context = nullptr;
		Vulkan_Swapchain* _swapchain = nullptr;

		FrameData _frames[Vulkan_RenderContext::MaxFramesInFlight];

		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<RHI_CommandFunc> _commandQueue;
		VkSemaphore _timelineSemaphore = VK_NULL_HANDLE;

		bool _frameRecording = false;
		u32 _imageIndex = 0;
		u32 _currentFrameIndex = 0;
		u64 _signalValue = 0;
		u64 _nextSignalValue = (Vulkan_RenderContext::MaxFramesInFlight + 1);

		std::vector<Memory::Ref<RHI_Image>> _bindlessImages;

		bool _recreateSwapchainNextFrame = false;
	};
}