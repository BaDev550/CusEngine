#pragma once
#include <Engine/Subsystem/Subsystem.h>

#include <Runtime/RHI/Swapchain/RHISwapchain.h>
#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Pipeline/RHIPipeline.h>

namespace CusEngine {
	class ENGINE_API RenderSubsystem final : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		[[nodiscard]] Runtime::RHI::Context* GetContext() { return _context; }
		[[nodiscard]] Runtime::RHI::Swapchain* GetSwapchain() { return _swapchain; }
	private:
		Runtime::RHI::Context* _context = nullptr;
		Runtime::RHI::Swapchain* _swapchain = nullptr;
	};
}