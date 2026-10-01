#pragma once

#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	enum class StaticSampler : u8 {
		PointClamp,
		PointWrap,
		LinearClamp,
		LinearWrap,
		LinearMirror,
		NearestClamp,
		NearestRepeat,
		AnisoClamp,
		AnisoMirror,
		ShadowCompare
	};
}