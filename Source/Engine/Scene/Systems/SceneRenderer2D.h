#pragma once

#include <Engine/Scene/System.h>
#include <Engine/Asset/Texture/Texture2D.h>

#include <Runtime/RHI/Pipeline/RHIPipeline.h>

namespace CusEngine {
	class ENGINE_API SceneRenderer2DSystem final : public System {
	public:
		virtual Runtime::Result OnCreate() override;
		virtual void OnUpdate(Scene& scene) override;
		virtual void OnDestroy() override;
	private:
		bool _recreateSwapchainNextFrame = false;

		struct SpritePushConstant {
			u32 textureID;
			u32 samplerID;
		};

		Runtime::RHI::Pipeline* _forwardPassPipeline = nullptr;

		Texture2D* _testSprite;
	};
}