#pragma once

#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
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