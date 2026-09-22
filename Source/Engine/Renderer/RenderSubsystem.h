#pragma once
#include <Engine/Subsystem/Subsystem.h>

#include <Runtime/RHI/Swapchain/RHISwapchain.h>
#include <Runtime/RHI/Command/RHICommands.h>
#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Pipeline/RHIPipeline.h>

namespace CusEngine {
	class ENGINE_API RenderSubsystem final : public Subsystem {
	public:
		virtual Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	private:
		RHI::Context* _context = nullptr;
		RHI::Commands* _commands = nullptr;
		RHI::Swapchain* _swapchain = nullptr;

		struct ImGuiPass {
			std::vector<RHI::Image*> colorAttachments; // FIXME
			u32 width;
			u32 height;
		} _imguiPass; 
	};
}