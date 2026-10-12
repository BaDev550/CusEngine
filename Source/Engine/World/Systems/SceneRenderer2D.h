#pragma once

#include <Engine/World/System.h> // TODO(0x): move this to another folder
#include <Engine/Asset/Texture/Texture2D.h>

#include <Runtime/RHI/Pipeline/RHIPipeline.h>

#include <Engine/Renderer/RenderPasses/ForwardRenderPass.h>
#include <Engine/Renderer/RenderPasses/WidgetRenderPass.h>

namespace Tourqe::Engine {
	class ENGINE_API SceneRenderer2DSystem final : public System {
	public:
		virtual Runtime::Result OnCreate() override;
		virtual void OnUpdate(Scene& scene) override;
		virtual void OnDestroy() override;
	private:
		ForwardRenderPass* _forwardRenderPass = nullptr;
		WidgetRenderPass* _widgetRenderPass = nullptr;
	};
}