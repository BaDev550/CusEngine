#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Command/RHICommandBuffer.h>
#include <functional>
#include <typeindex>
#include <unordered_set>

#include <Runtime/Reflection/ReflectObject.h>
#include <Runtime/Reflection/Type.h>

namespace Tourqe::Engine {
	class RenderSubsystem;

	TCLASS()
	class RenderPass : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(RenderPass)
	public:
		using RenderPassSubmit = std::function<void(Runtime::RHI::CommandBuffer* cmd)>; // TODO(0x): add rendering list
		RenderPass() = default;
		virtual ~RenderPass() = default;
		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;

		virtual void OnInitialized() {}

		virtual void Begin(Runtime::RHI::CommandBuffer* cmd) = 0;
		virtual void End(Runtime::RHI::CommandBuffer* cmd) = 0;
		void Submit(RenderPassSubmit submit) { _submitList.push_back(std::move(submit)); }
		void SubmitQueueForRender(Runtime::RHI::CommandBuffer* cmd) { for (auto& submit : _submitList) { submit(cmd); } _submitList.clear(); }
		void SetRenderSubsystem(RenderSubsystem* system) { _renderSystem = system; }

		[[nodiscard]] const std::vector<std::string>& GetDependencies() const { return _dependencyList; }
	private:
		RenderSubsystem* _renderSystem = nullptr;
		std::vector<RenderPassSubmit> _submitList;
		std::vector<std::string> _dependencyList;
	protected:
		[[nodiscard]] RenderSubsystem* GetRenderSystem() const { return _renderSystem; }

		template<typename TPass> 
			requires std::is_base_of_v<Runtime::Reflection::ReflectObject, TPass>
		void Depends() {
			_dependencyList.push_back(TPass::GetStaticClassName().data());
		}

		template<typename TPass>
			requires std::is_base_of_v<Runtime::Reflection::ReflectObject, TPass>
		const std::string_view& GetDependencyName() const { return _dependencyList[TPass::GetStaticClassName()]; }
	};
}