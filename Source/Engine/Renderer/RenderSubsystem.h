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
		virtual bool OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	};
}