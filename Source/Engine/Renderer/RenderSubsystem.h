#pragma once
#include "Subsystem/Subsystem.h"

#include "Graphics/RHI/RHI.h"
#include "Graphics/RHI/RHI_Swapchain.h"
#include "Graphics/RHI/RHI_RenderCommands.h"
#include "Graphics/RHI/RHI_RenderContext.h"
#include "Graphics/RHI/RHI_Image.h"
#include "Graphics/RHI/RHI_Buffer.h"
#include "Graphics/RHI/RHI_Pipeline.h"

#include "Graphics/Texture2D.h"
#include "Graphics/Framebuffer.h"

namespace CusEngine {
	class RenderSubsystem final : public Subsystem {
	public:
		virtual bool OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	private:
		RHI::RenderCommands* _commands = nullptr;
	};
}