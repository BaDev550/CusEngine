#include "RenderGraph.h"

#include <Engine/Core/Engine.h>
#include <Engine/Renderer/RenderSubsystem.h>

#include <Runtime/Memory/Memory.h>

namespace Tourqe::Engine {
	RenderGraph::~RenderGraph() {
		ClearPasses();
	}

	RenderGraphHandle RenderGraph::ImportImage(const std::string& name, Runtime::RHI::Image* img, Runtime::RHI::ImageLayout inital, Runtime::RHI::ImageLayout final) {
		RenderGraphResourceContainer::RenderGraphResource resource{};
		resource.name = std::move(name);
		resource.state = AssetState::Loaded;
		resource.physicalAttachment = img;
		resource.initalState = inital;
		resource.finalState = final;
		u32 id = static_cast<u32>(_resources.size());
		_resources.push_back(std::move(resource));
		return { id };
	}

	RenderGraphHandle RenderGraph::CreateImage(const std::string& name, const Runtime::RHI::ImageDesc& desc) {
		RenderGraphResourceContainer::RenderGraphResource resource{};
		resource.name = std::move(name);
		resource.desc = desc;
		u32 id = static_cast<u32>(_resources.size());
		_resources.push_back(std::move(resource));
		return { id };
	}

	void RenderGraph::AddPass(std::string name, SetupFn setup, RenderPassExecuteFn execute) {
		RenderPass* pass = Runtime::Mem::Allocator::Construct<RenderPass>(std::move(name), std::move(execute));
		RenderPassBuilder builder{ pass };
		setup(builder);
		_passes.push_back(std::move(pass));
	}

	void RenderGraph::Compile() {
		std::vector<b8> needed(_resources.size(), false);
		for (i32 i = static_cast<i32>(_passes.size()) - 1; i >= 0; i--) {
			RenderPass* pass = _passes[i];
			if (!pass) continue;

			b8 live = pass->HasSideEffects();
			for (const RenderPassAccess& a : pass->GetAccesses()) {
				if (a.write && ((_resources[a.handle.id].state == AssetState::Loaded) || needed[a.handle.id])) live = true;
			}
			pass->SetLife(live);
			if (!live) continue;
			for (const RenderPassAccess& a : pass->GetAccesses()) needed[a.handle.id] = true;
		}

		std::vector<Runtime::RHI::ImageLayout> currentLayouts(_resources.size());
		for (usize i = 0; i < _resources.size(); i++) {
			currentLayouts[i] = (_resources[i].state == AssetState::Loaded) ? _resources[i].initalState : Runtime::RHI::ImageLayout::Undefined;
			_resources[i].firstUse = -1;
			_resources[i].lastUse = -1;
		}
		std::vector<bool> lastWrite(_resources.size(), false);

		for (i32 i = 0; i < static_cast<i32>(_passes.size()); i++) {
			RenderPass* pass = _passes[i];
			if (!pass) continue;

			pass->GetBarriers().clear();

			if (!pass->IsAlive()) continue;
			for (const RenderPassAccess& a : pass->GetAccesses()) {
				RenderGraphResourceContainer::RenderGraphResource& res = _resources[a.handle.id];
				const bool stateChanged = currentLayouts[a.handle.id] != a.layout;
				const bool hazard = lastWrite[a.handle.id] || a.write;
				if (stateChanged || hazard) {
					pass->AddBarrier({ a.handle, currentLayouts[a.handle.id], a.layout });
				}
				currentLayouts[a.handle.id] = a.layout;
				lastWrite[a.handle.id] = a.write;
				if (res.firstUse < 0) res.firstUse = i;
				res.lastUse = i;
			}
		}

		_finalBarriers.clear();
		for (usize i = 0; i < _resources.size(); i++) {
			if ((_resources[i].state == AssetState::Loaded) && currentLayouts[i] != _resources[i].finalState && _resources[i].lastUse >= 0) {
				_finalBarriers.push_back({ { static_cast<u32>(i) }, currentLayouts[i], _resources[i].finalState });
			}
		}
	}

	void RenderGraph::Execute(Runtime::RHI::CommandBuffer * cmd) {
		auto* renderSubsystem = Engine::Get()->GetSubsystem<RenderSubsystem>();

		for (auto& resource : _resources) {
			if ((resource.state == AssetState::InDisk) && resource.lastUse >= 0) {
				resource.physicalAttachment = renderSubsystem->GetContext()->CreateImage(resource.desc);
				renderSubsystem->Track(resource.physicalAttachment);
			}
		}

		RenderGraphResourceContainer view(&_resources);
		for (auto pass : _passes) {
			if (!pass) continue;

			if (!pass->IsAlive()) continue;
			for (const RenderPassBarrier& barrier : pass->GetBarriers()) EmitBarrier(cmd, barrier);
			pass->ExecFunc(cmd, view);
		}
		for (const RenderPassBarrier& barrier : _finalBarriers) EmitBarrier(cmd, barrier);
	}

	void RenderGraph::EmitBarrier(Runtime::RHI::CommandBuffer * cmd, const RenderPassBarrier & barrier) {
		cmd->TransitionImageLayout(_resources[barrier.handle.id].physicalAttachment, barrier.after);
	}

	void RenderGraph::ClearPasses() {
		for (auto& pass : _passes) {
			Runtime::Mem::Allocator::Destroy(pass);
			pass = nullptr;
		}
		_finalBarriers.clear();
		_resources.clear();
	}
}