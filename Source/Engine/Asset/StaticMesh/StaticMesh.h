#pragma once
#include <Engine/Asset/Asset.h>

namespace CusEngine {
	CUS_CLASS()
	class ENGINE_API StaticMesh : public Asset {
		REFLECT_CLASS(StaticMesh)
	public:
		StaticMesh() {}
	private:
		u32 vertexCount{ 0 };
		u32 indexCount{ 0 };
	};
}