#include "RenderPass.h"

#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

namespace Tourqe::Engine {
	void RenderPass::Submit(RenderPassSubmit submit) {
		if (_owningRenderSubsystem->CanSubmitRenderCommands()) {
			_submitList.push_back(std::move(submit));
		}
		else {
			_submitList.clear();
		}
	}
}