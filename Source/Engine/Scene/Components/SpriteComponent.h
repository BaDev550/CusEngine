#pragma once

#include <Engine/Scene/Components/BaseComponent.h>
#include <Runtime/Definitions/UUID.h>

namespace Tourqe::Engine {
	TCLASS()
	class SpriteComponent : public BaseComponent {
		GENERATE_CLASS(SpriteComponent)
	public:
		TPROP()
		Runtime::UUID TextureAssetHandle;

		TPROP()
		bool Visible;
	};
}