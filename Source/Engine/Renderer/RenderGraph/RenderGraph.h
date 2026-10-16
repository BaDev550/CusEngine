#pragma once

#include <Engine/Renderer/RenderGraph/RenderPass.h>

namespace Tourqe::Engine {
	class RenderGraph final {
	public:
		using SetupFn = std::function<void(RenderPassBuilder&)>;
		~RenderGraph();

		RenderGraphHandle ImportImage(const std::string& name, Runtime::RHI::Image* img, Runtime::RHI::ImageLayout inital, Runtime::RHI::ImageLayout final);
		RenderGraphHandle CreateImage(const std::string& name, const Runtime::RHI::ImageDesc& desc);
		void AddPass(std::string name, SetupFn setup, RenderPassExecuteFn execute);

		void Compile();
		void Execute(Runtime::RHI::CommandBuffer* cmd);
	private:
		void EmitBarrier(Runtime::RHI::CommandBuffer* cmd, const RenderPassBarrier& barrier);

		std::vector<RenderPass*> _passes;
		std::vector<RenderPassBarrier> _finalBarriers;
		std::vector<RenderGraphResourceContainer::RenderGraphResource> _resources;
	};
}