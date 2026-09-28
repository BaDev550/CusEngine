#pragma once

#include <Runtime/RHI/Command/RHICommands.h>

#include <Runtime/Vulkan/VulkanContext.h>
#include <Runtime/Vulkan/VulkanSwapchain.h>

namespace CusEngine::RHI {
	class VulkanCommands : public Commands {
	public:
		VulkanCommands(const CommandsDesc& desc);
		virtual ~VulkanCommands();

		void CreateCommandPool();
		void CreateTimelineSemaphore();
		void CreateRenderFinishedSemaphore();
		
		virtual std::string_view GetObjectDebugName() const override { return "rhi_object_vulkan_commands"; }

		virtual void BeginFrame() override final;
		virtual void EndFrame() override final;
		virtual void BeginImGui() override final;
		virtual void EndImGui() override final;
		virtual void Submit(CommandFunc func) override final;
		virtual void Track(RHIObject* object) override final;
		virtual void Wait() override final;

		virtual void BeginDynamicRendering(std::vector<Image*> colorAttachments, Image* depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) override final;
		virtual void EndDynamicRendering() override final;

		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) override;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) override;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) override;
		virtual const CommandsDesc& GetDesc() const { return _desc; }

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept override final;
	private:
		CommandsDesc _desc;

		struct FrameData {
			VkCommandPool CommandPool = VK_NULL_HANDLE;
			VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
			VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
			std::vector<RHIObject*> TrackedObjects;
		} _frames[VulkanContext::MaxFramesInFlight];

		FrameData* GetCurrentFrameData();

		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<CommandFunc> _commandQueue;
		VkSemaphore _timelineSemaphore = VK_NULL_HANDLE;

		bool _frameRecording = false;
		u32 _imageIndex = 0;
		u32 _currentFrameIndex = 0;
		u64 _signalValue = 0;
		u64 _nextSignalValue = (VulkanContext::MaxFramesInFlight + 1);

		bool _recreateSwapchainNextFrame = false;
	};
}