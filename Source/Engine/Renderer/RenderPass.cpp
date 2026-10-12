#include "RenderPass.h"

#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSystem.h>

namespace Tourqe::Engine {
	void RenderPass::RegisterClassToDependencyList(const std::string& name)
	{
		auto* reflectionSystem = Engine::Get()->GetReflectionSystem();
		if (Runtime::Reflection::Type* type = reflectionSystem->GetType(name)) {
			_dependencyList[type->GetName()] = type;
		}
		else {
			Logger::Error("RenderPass", "Failed to register class to dependency list: class is not registered");
			return;
		}
	}

	void RenderPass::Begin() {
		for (auto [name, dependency] : _dependencyList) { // O(N) and every frame fuck:p
			RenderPass* pass = dynamic_cast<RenderPass*>(dependency);
			if (!pass->_recording) {
				Logger::Error(GetClassName(), "Failed to begin rendering {} dependency not active", name);
				return;
			}
		}
		_recording = true;
	}

	void RenderPass::End() {
		_recording = false;
	}
}