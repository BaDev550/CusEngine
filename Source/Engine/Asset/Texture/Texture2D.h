#pragma once

#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Image/RHIImage.h>

namespace CusEngine {
	CCLASS()
	class ENGINE_API Texture2D final : public Asset { // TEMP CLASS
		GENERATE_CLASS(Texture2D);
	public:
		Texture2D() = default;
		Texture2D(const Runtime::RHI::ImageDesc& desc);
		~Texture2D();

		Runtime::RHI::Image* _image = nullptr;
	};
}