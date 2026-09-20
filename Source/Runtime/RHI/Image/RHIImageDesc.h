#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/RHI/Common/RHIFormat.h>
#include <Runtime/RHI/Image/RHIImageUsage.h>
#include <Runtime/RHI/Image/RHIImageLayout.h>
#include <Runtime/RHI/Image/RHIImageTileMode.h>

namespace CusEngine::RHI {
	struct ImageDesc {
		u32 width = 0;
		u32 height = 0;
		Format format = Format::Undefined;
		ImageUsage usage = ImageUsage::None;
		ImageTileMode tileMode = ImageTileMode::Repeat;
		ImageLayout layout = ImageLayout::Undefined;
	};
}