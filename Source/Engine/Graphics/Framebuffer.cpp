#include "Framebuffer.h"
#if 0
#include "RHI/RHI_Utils.h"
#include "RHI/RHI_RenderCommands.h"
#include "RHI/RHI.h"

#include "Core/Engine.h"

namespace CusEngine {
	Framebuffer::Framebuffer(RHI::RenderCommands* commands, const FramebufferDesc& desc) : RenderObject(commands), _desc(desc) {
		for (const auto& colorFormat : _desc.ColorFormats) {
			RHI::ImageDesc desc{};
			desc.width = _desc.width;
			desc.height = _desc.height;
			desc.usage = RHI::ImageUsage::ColorAttachment;
			desc.tileMode = RHI::ImageTileMode::Optimal;
			desc.layout = RHI::ImageLayout::ColorAttachment;
			desc.format = colorFormat;
			//_colorAttachments.push_back(Mem::Ref<RHI::Image>(RHI::CreateImage(commands, desc)));
		}

		for (const auto& depthFormat : _desc.DepthFormats) {
			RHI::ImageDesc desc{};
			desc.width = _desc.width;
			desc.height = _desc.height;
			desc.usage = RHI::ImageUsage::DepthStencilAttachment;
			desc.tileMode = RHI::ImageTileMode::Optimal;
			desc.layout = RHI::ImageLayout::DepthAttachment;
			desc.format = depthFormat;
			//_depthAttachments.push_back(Mem::Ref<RHI::Image>(RHI::CreateImage(commands, desc)));
		}
		Logger::Info(GetDebugName(), "Color {} and Depth {} images are created", _colorAttachments.size(), _depthAttachments.size());
	}

	Framebuffer::~Framebuffer() {
		_colorAttachments.clear();
		_depthAttachments.clear();
	}
	
	FramebufferDesc::FramebufferDesc(const std::initializer_list<RHI::ImageFormat>& attachments) {
		for (auto attachment : attachments) {
			if (RHI::Utils::IsFormatDepth(attachment)) {
				DepthFormats.push_back(attachment);
			}
			else {
				ColorFormats.push_back(attachment);
			}
		}
	}
}
#endif