#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/UUID.h>
#include <Runtime/RHI/Image/RHIImageFilter.h>

namespace CusEngine::RHI {
	struct ImageSampler {
		ImageFilter filter = ImageFilter::Nearest;
		ImageTileMode tileMode = ImageTileMode::Repeat;
	};
}