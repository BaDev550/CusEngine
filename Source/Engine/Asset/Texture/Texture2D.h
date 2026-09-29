#pragma once

#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Image/RHIImage.h>

namespace CusEngine {
	class ENGINE_API Texture2D final : public Asset { // TEMP CLASS
		REFLECT_CLASS();
	public:
		Texture2D(const RHI::ImageDesc& desc);
		~Texture2D();

		RHI::Image* _image = nullptr;
	};
}