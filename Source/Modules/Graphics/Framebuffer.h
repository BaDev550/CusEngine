#pragma once

#include "RHI/RHI_Image.h"
#include "Memory/Memory.h"

namespace Graphics {
	struct FramebufferDesc {
		u32 Width = 1;
		u32 Height = 1;

		FramebufferDesc(const std::initializer_list<RHI_Format>& attachments);
	
		std::vector<RHI_Format> ColorFormats;
		std::vector<RHI_Format> DepthFormats;
	};

	class Framebuffer final : public Memory::RefCounted {
	public:
		Framebuffer(const FramebufferDesc& desc);
		~Framebuffer();

		void SetDebugName(const std::string& name) { _debugName = name; }
		[[nodiscard]] const std::string& GetDebugName() const { return _debugName; }

		[[nodiscard]] const Memory::Ref<RHI_Image>& GetColorAttachment(u32 index) const { return _colorAttachments[index]; }
		[[nodiscard]] const Memory::Ref<RHI_Image>& GetDepthAttachment(u32 index) const { return _depthAttachments[index]; }
		[[nodiscard]] const std::vector<Memory::Ref<RHI_Image>>& GetColorAttachments() const { return _colorAttachments; }
		[[nodiscard]] const std::vector<Memory::Ref<RHI_Image>>& GetDepthAttachments() const { return _depthAttachments; } // hehe
	private:
		std::string _debugName = "Framebuffer";

		FramebufferDesc _desc;
		std::vector<Memory::Ref<RHI_Image>> _colorAttachments;
		std::vector<Memory::Ref<RHI_Image>> _depthAttachments;
	};
}