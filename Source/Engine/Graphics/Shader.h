#pragma once

#if 0
#include "Core/Core.h"
//#include "ResourceManager/Resource.h"
#include <vector>

namespace Graphics {
	enum class ShaderStage : u8 {
		Vertex,
		Fragment,
		Compute
	};

	using ShaderBytecode = std::vector<u32>;

	//class Shader final : public Resource {
	//public:
	//	virtual ResourceType GetResourceType() const override { return ResourceType::Shader; }
	//};
}
#endif