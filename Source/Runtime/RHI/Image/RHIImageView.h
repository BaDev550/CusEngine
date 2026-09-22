#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class ImageViewType : u8 {
		None = 0,
		Image2D,
		Image3D
	};

	struct ImageView {
		ImageViewType type = ImageViewType::None;
		bool mipCount = 0;
	};
}