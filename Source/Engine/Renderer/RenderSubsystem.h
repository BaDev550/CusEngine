#pragma once
#include <Engine/Subsystem/Subsystem.h>
#include <Engine/Asset/Shader/Shader.h>

#include <Runtime/RHI/Swapchain/RHISwapchain.h>
#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Pipeline/RHIPipeline.h>

#include <Runtime/RHI/Command/RHICommandPool.h>
#include <Runtime/RHI/Command/RHICommandBuffer.h>
#include <Runtime/RHI/Sync/RHIFence.h>
#include <Runtime/RHI/Queue/RHIQueue.h>

namespace CusEngine {
	using namespace Runtime;

	using CommandFunc = std::function<void(RHI::CommandBuffer* cmd)>;

	class Texture2D;

	class ENGINE_API RenderSubsystem final : public Subsystem {
	public:
		constexpr static u32 MaxFramesInFlight = 2;

		virtual Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		void BeginFrame();
		void EndFrame();
		void Submit(CommandFunc func);
		void Track(RHI::Object* object);

		void BeginSwapchainPass();
		void EndSwapchainPass();

		u32 GetImageIndex() const { return _imageIndex; }
		[[nodiscard]] RHI::Context* GetContext() { return _context; }
		[[nodiscard]] RHI::Swapchain* GetSwapchain() { return _swapchain; }
	private:
		RHI::Context* _context = nullptr;
		RHI::Swapchain* _swapchain = nullptr;

		struct FrameData {
			RHI::CommandPool* commandPool = nullptr;
			RHI::CommandBuffer* commandBuffer = nullptr;
			RHI::Fence* imageAvailableFence = nullptr;

			std::vector<RHI::Object*> trackedObjects;
		} _frames[MaxFramesInFlight];

		std::vector<CommandFunc> _commandQueue;
		std::vector<RHI::Fence*> _renderFinishedFences;
		RHI::Fence* _timelineFence = nullptr;

		bool _frameRecording = false;
		u32 _imageIndex = 0;
		u32 _currentFrameIndex = 0;
		u64 _signalValue = 0;
		u64 _nextSignalValue = (MaxFramesInFlight + 1);

		bool _recreateSwapchainNextFrame = false;

		RHI::Image* _defaultWhiteImage = nullptr;
	public:
		[[nodiscard]] FrameData* GetCurrentFrameData();
	};
}