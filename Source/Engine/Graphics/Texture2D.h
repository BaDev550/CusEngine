#pragma once

#if 0
#include "RHI/RHI.h"
#include "RenderObject.h"

namespace CusEngine {
	struct TextureDesc {
		u32 Width = 1;
		u32 Height = 1;
		RHI::ImageFormat Format = RHI::ImageFormat::Undefined;
	};

	class Texture2D final : public RenderObject {
	public:
		Texture2D(RHI::RenderCommands* commands, const TextureDesc& desc, u8* data);
		~Texture2D();

		u32 GetBindlessID();
		Mem::Ref<RHI::Image>& GetImage() { return _image; }
	private:
		TextureDesc _desc;
		u32 _bindlessID = u32_max;

		Mem::Ref<RHI::Image> _image;
	};
}
#endif