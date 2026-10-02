#pragma once

#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	enum class StaticSampler : u8 {
		PointClamp = 0,
		PointWrap,
		LinearClamp,
		LinearWrap,
		LinearMirror,
		NearestClamp,
		NearestRepeat,
		AnisoClamp,
		AnisoMirror,
		ShadowCompare,
		COUNT
	};
}