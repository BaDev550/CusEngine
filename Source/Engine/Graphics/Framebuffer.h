#pragma once

#include "Graphics/RHI/RHI_Image.h"
#include "Core/Memory.h"
#include "Core/Ref.h"

namespace CusEngine {
	struct FramebufferDesc {
		u32 width = 1;
		u32 height = 1;

		FramebufferDesc(const std::initializer_list<RHI::ImageFormat>& attachments);

		std::vector<RHI::ImageFormat> ColorFormats;
		std::vector<RHI::ImageFormat> DepthFormats;
	};

	class Framebuffer final : public RenderObject {
	public:
		Framebuffer(RHI::RenderCommands* commands, const FramebufferDesc& desc);
		~Framebuffer();

		void SetDebugName(const std::string& name) { _debugName = name; }
		[[nodiscard]] const std::string& GetDebugName() const { return _debugName; }

		[[nodiscard]] const Mem::Ref<RHI::Image>& GetColorAttachment(u32 index) const { return _colorAttachments[index]; }
		[[nodiscard]] const Mem::Ref<RHI::Image>& GetDepthAttachment(u32 index) const { return _depthAttachments[index]; }
		[[nodiscard]] const std::vector<Mem::Ref<RHI::Image>>& GetColorAttachments() const { return _colorAttachments; }
		[[nodiscard]] const std::vector<Mem::Ref<RHI::Image>>& GetDepthAttachments() const { return _depthAttachments; } // hehe
	private:
		std::string _debugName = "Framebuffer";

		FramebufferDesc _desc;
		std::vector<Mem::Ref<RHI::Image>> _colorAttachments;
		std::vector<Mem::Ref<RHI::Image>> _depthAttachments;
	};
}