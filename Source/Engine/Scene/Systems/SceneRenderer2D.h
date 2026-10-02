#pragma once

#include <Engine/Renderer/RenderSubsystem.h>

namespace CusEngine {
	class ENGINE_API SceneRenderer2DSubsystem final : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	private:
		struct SpritePushConstant {
			u32 textureID;
			u32 samplerID;
		};

		Runtime::RHI::Pipeline* _forwardPassPipeline = nullptr;

		Texture2D* _testSprite;
	};
}