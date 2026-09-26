#pragma once

#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Image/RHIImage.h>

namespace CusEngine {
	class ENGINE_API Texture2D final : public Asset {
		REFLECT_CLASS();
	public:
		Texture2D() = default;
	private:
		u8* _data = nullptr;
		RHI::Image* _image = nullptr;
	};
}