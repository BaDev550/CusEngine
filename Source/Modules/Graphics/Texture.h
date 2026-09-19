#pragma once

#include "RHI/RHI.h"

namespace Graphics {
	struct TextureDesc {
		u32 Width = 1;
		u32 Height = 1;
		RHI_Format Format = RHI_Format::Undefined;
	};

	class Texture2D final : public Memory::RefCounted {
	public:
		Texture2D(const TextureDesc& desc, u8* data);
		~Texture2D();

		u32 GetBindlessID();
		Memory::Ref<RHI_Image>& GetImage() { return _image; }
	private:
		TextureDesc _desc;
		u32 _bindlessID = u32_max;
		Memory::Ref<RHI_Image> _image;
	};
}