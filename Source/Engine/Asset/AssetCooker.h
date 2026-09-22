#pragma once

#include <Engine/Core/Object.h>
#include <Engine/Core/Result.h>
#include <Engine/Asset/AssetSource.h>

namespace CusEngine {
	CUS_CLASS()
	class ENGINE_API AssetCooker : public Object {
		REFLECT_CLASS(AssetCooker)
	public:
		virtual ~AssetCooker() = default;

		virtual Result Cook(AssetSource& source) = 0;
	};
}