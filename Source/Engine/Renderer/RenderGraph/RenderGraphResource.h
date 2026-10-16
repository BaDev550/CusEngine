#pragma once

#include <Runtime/Definitions/Types.h>

#include <Engine/Renderer/RenderGraph/RenderGraphHandle.h>
#include <Engine/Asset/AssetLoadStat.h>

#include <Runtime/RHI/Image/RHIImageDesc.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <string>
#include <vector>

namespace Tourqe::Engine {
	class RenderGraphResourceContainer final {
	public:
		struct RenderGraphResource {
			std::string name;
			AssetState state = AssetState::InDisk;
			Runtime::RHI::ImageDesc desc;
			Runtime::RHI::Image* physicalAttachment = nullptr;
			Runtime::RHI::ImageLayout initalState = Runtime::RHI::ImageLayout::Undefined;
			Runtime::RHI::ImageLayout finalState = Runtime::RHI::ImageLayout::Undefined;
			i32 firstUse = -1;
			i32 lastUse = -1;
		};
	public:
		explicit RenderGraphResourceContainer(const std::vector<RenderGraphResource>* resources) : _resources(resources) {}
		[[nodiscard]] Runtime::RHI::Image* GetImage(RenderGraphHandle handle) const { return (*_resources)[handle.id].physicalAttachment; }
	private:
		const std::vector<RenderGraphResource>* _resources;
	};
}