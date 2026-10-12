#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Command/RHICommandBuffer.h>
#include <functional>
#include <typeindex>
#include <unordered_set>

#include <Runtime/Reflection/ReflectObject.h>
#include <Runtime/Reflection/Type.h>

namespace Tourqe::Engine {
	TCLASS()
	class RenderPass : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(RenderPass)
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Begin();
		virtual void End();

		//void SetVec3Param(); hanlde by RTTR
		//void SetVec2Param();
		//void SetMat4Param();
	protected:
		bool _recording = false;
		std::unordered_map<std::string, Runtime::Reflection::Type*> _dependencyList;

		template<typename TPass> 
			requires std::is_base_of_v<Runtime::Reflection::ReflectObject, TPass>
		void Depends() {
			std::string name = TPass::GetStaticClassName().data();
			if (_dependencyList.contains(name)) {
				Logger::Info("RenderPass", "Already has this dependency.");
				return;
			}
			RegisterClassToDependencyList(name);
		}

		template<typename TPass>
			requires std::is_base_of_v<Runtime::Reflection::ReflectObject, TPass>
		Runtime::Reflection::Type* GetDependency() { return _dependencyList[TPass::GetStaticClassName()]; }
	private:
		void RegisterClassToDependencyList(const std::string& name);
	};
}