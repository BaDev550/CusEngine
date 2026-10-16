#pragma once

#include <Engine/World/ECS/EntityDefinitions.h>
#include <Engine/Core/Core.h>

namespace Tourqe::Engine {
	class ENGINE_API EntityFactory final {
	public:
		EntityFactory();
		~EntityFactory() = default;

		EntityHandle Create();
		EntityHandle GetHandleAt(usize index);
		b8 Destroy(EntityHandle handle);

		b8 IsValid(EntityHandle handle) const;

		Signature* GetSignatureOf(EntityHandle handle);
		const Signature* GetSignatureOf(EntityHandle handle) const;

		void EnsureIndex(u32 index);
	private:
		std::vector<u32> _generations;
		std::vector<u32> _freeList;
		std::vector<b8> _alive;
		std::vector<Signature> _signatures;
		u32 _nextIndex = 0;
	};
}