#include "Framebuffer.h"
#include "RHI/RHI_Utils.h"
#include "RHI/RHI_RenderCommands.h"
#include "RHI/RHI.h"

#include "Core/Engine.h"

namespace Graphics {
	Framebuffer::Framebuffer(const FramebufferDesc& desc) : _desc(desc) {
		RHI_RenderCommands* commands = Engine::Get()->GetRenderer()->GetRenderCommands(); // TODO(0x): use DI

		for (const auto& colorFormat : _desc.ColorFormats) {
			RHI_ImageDesc desc{};
			desc.Width = _desc.Width;
			desc.Height = _desc.Height;
			desc.Usage = RHI_ImageUsage::ColorAttachment;
			desc.TileMode = RHI_ImageTileMode::Optimal;
			desc.Layout = RHI_ImageLayout::ColorAttachment;
			desc.Format = colorFormat;
			_colorAttachments.push_back(Memory::Ref<RHI_Image>(RHI_CreateImage(commands, desc)));
		}

		for (const auto& depthFormat : _desc.DepthFormats) {
			RHI_ImageDesc desc{};
			desc.Width = _desc.Width;
			desc.Height = _desc.Height;
			desc.Usage = RHI_ImageUsage::DepthStencilAttachment;
			desc.TileMode = RHI_ImageTileMode::Optimal;
			desc.Layout = RHI_ImageLayout::DepthAttachment;
			desc.Format = depthFormat;
			_depthAttachments.push_back(Memory::Ref<RHI_Image>(RHI_CreateImage(commands, desc)));
		}
		Logger::Info(GetDebugName(), "Color {} and Depth {} images are created", _colorAttachments.size(), _depthAttachments.size());
	}

	Framebuffer::~Framebuffer() {
		_colorAttachments.clear();
		_depthAttachments.clear();
	}
	
	FramebufferDesc::FramebufferDesc(const std::initializer_list<RHI_Format>& attachments) {
		for (auto attachment : attachments) {
			if (Utils::RHI_IsFormatDepth(attachment)) {
				DepthFormats.push_back(attachment);
			}
			else {
				ColorFormats.push_back(attachment);
			}
		}
	}
}