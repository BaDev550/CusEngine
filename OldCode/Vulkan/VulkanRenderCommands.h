#pragma once

#include "Graphics/RHI/RHI_RenderCommands.h"

#include "VulkanRenderContext.h"
#include "VulkanSwapchain.h"

namespace CusEngine::RHI {
	class Vulkan_RenderCommands : public RenderCommands {
	public:
		struct FrameData {
			VkCommandPool CommandPool = VK_NULL_HANDLE;
			VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
			VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
			std::vector<Mem::Ref<RenderObject>> TrackedObjects;
		};

		Vulkan_RenderCommands(Vulkan_RenderContext* context, Vulkan_Swapchain* swapchain);
		virtual ~Vulkan_RenderCommands();

		virtual void BeginFrame() override final;
		virtual void EndFrame() override final;
		virtual void Submit(CommandFunc func) override final;
		virtual void Track(const Mem::Ref<RenderObject>& object) override final;
		virtual void Wait() override final;

		virtual void BeginDynamicRendering(std::vector<Mem::Ref<Image>> colorAttachments, Mem::Ref<Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) override final;
		virtual void EndDynamicRendering() override final;

		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) override;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) override;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) override;
		virtual u32 RegisterBindlessImage(const Mem::Ref<Image>& image) override;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept override final;
		virtual [[nodiscard]] Swapchain* GetTargetSwapchain() const override final;
		virtual [[nodiscard]] RenderContext* GetTargetRenderContext() const override final;

		[[nodiscard]] Vulkan_RenderContext* GetVkContext() const { return _context; }
	private:
		[[nodiscard]] FrameData* GetCurrentFrameData();

		Vulkan_RenderContext* _context = nullptr;
		Vulkan_Swapchain* _swapchain = nullptr;

		FrameData _frames[Vulkan_RenderContext::MaxFramesInFlight];

		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<CommandFunc> _commandQueue;
		VkSemaphore _timelineSemaphore = VK_NULL_HANDLE;

		bool _frameRecording = false;
		u32 _imageIndex = 0;
		u32 _currentFrameIndex = 0;
		u64 _signalValue = 0;
		u64 _nextSignalValue = (Vulkan_RenderContext::MaxFramesInFlight + 1);

		std::vector<Mem::Ref<Image>> _bindlessImages;

		bool _recreateSwapchainNextFrame = false;
	};
}