#pragma once

#include <Engine/Core/Object.h>

namespace CusEngine {
	CUS_CLASS()
	class AssetCooker : public Object {
		REFLECT_CLASS(AssetCooker)
	public:
		virtual ~AssetCooker() = 0;

		virtual bool Cook() = 0;
	};
}