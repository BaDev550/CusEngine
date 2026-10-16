#pragma once

#include <Engine/Renderer/RenderGraph/RenderGraphResource.h>
#include <Runtime/RHI/Command/RHICommandBuffer.h>
#include <functional>

namespace Tourqe::Engine {
	using RenderPassExecuteFn = std::function<void(Runtime::RHI::CommandBuffer*, const RenderGraphResourceContainer&)>;

	struct RenderPassBarrier {
		RenderGraphHandle handle;
		Runtime::RHI::ImageLayout before = Runtime::RHI::ImageLayout::Undefined;
		Runtime::RHI::ImageLayout after = Runtime::RHI::ImageLayout::Undefined;
	};

	struct RenderPassAccess {
		RenderGraphHandle handle;
		Runtime::RHI::ImageLayout layout;
		b8 write;
	};

	class RenderPass final {
	public:
		RenderPass(const std::string& name, RenderPassExecuteFn execFunc) : _name(name), _exetureFunc(execFunc) {}

		const std::string GetName() const { return _name; }

		[[nodiscard]] std::vector<RenderPassAccess>& GetAccesses() { return _accesses; }
		[[nodiscard]] std::vector<RenderPassBarrier>& GetBarriers() { return _barriers; }
		[[nodiscard]] __forceinline b8 HasSideEffects() const { return _hasSideEffect; }
		[[nodiscard]] __forceinline b8 IsAlive() const { return _alive; }

		void ExecFunc(Runtime::RHI::CommandBuffer* cmd, const RenderGraphResourceContainer& container) const { _exetureFunc(cmd, container); }
		
		void AddBarrier(const RenderPassBarrier& barrier) { _barriers.push_back(barrier); }
		void SetLife(b8 alive) { _alive = alive; }
	private:
		std::string _name;
		b8 _hasSideEffect;
		b8 _alive;
		RenderPassExecuteFn _exetureFunc;
		std::vector<RenderPassAccess> _accesses;
		std::vector<RenderPassBarrier> _barriers;
	};

	class RenderPassBuilder final {
	public:
		void Read(RenderGraphHandle handle, Runtime::RHI::ImageLayout layout) { _pass->GetAccesses().emplace_back(handle, layout, false); }
		void Write(RenderGraphHandle handle, Runtime::RHI::ImageLayout layout) { _pass->GetAccesses().emplace_back(handle, layout, true); }
	private:
		explicit RenderPassBuilder(RenderPass* pass) : _pass(pass) {}

		RenderPass* _pass = nullptr;
		friend class RenderGraph;
	};
}