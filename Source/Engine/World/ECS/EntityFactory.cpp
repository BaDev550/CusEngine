#include "EntityFactory.h"
#include <Runtime/Definitions/Logger.h>

namespace Tourqe::Engine {
	EntityFactory::EntityFactory() {
		_generations.resize(MaxEntities);
		_signatures.resize(MaxEntities);
		_alive.resize(MaxEntities);
	}

	EntityHandle EntityFactory::Create() {
		u32 index = 0;

		if (!_freeList.empty()) {
			index = _freeList.back();
			_freeList.pop_back();
		}
		else {
			index = _nextIndex;
			_nextIndex++;

			EnsureIndex(index);
		}

		_alive[index] = true;
		return EntityHandle{ index, _generations[index] };
	}

	void EntityFactory::EnsureIndex(u32 index)
	{
		if (index < _generations.size())
			return;
		usize newCount = _generations.size();
		if (newCount == 0)
			newCount = MaxEntities;

		while (newCount <= index)
			newCount *= 2;

		_generations.resize(newCount);
		_alive.resize(newCount);
		_signatures.resize(newCount);
	}

	b8 EntityFactory::Destroy(EntityHandle handle) {
		if (!IsValid(handle)) {
			Logger::Warn("EntityFactory", "Cannot destroy entity: invalid handle");
			return false;
		}

		const u32 index = handle.index;
		_alive[index] = false;
		_signatures[index].reset();
		_generations[index]++;
		_freeList.push_back(index);
		return true;
	}

	b8 EntityFactory::IsValid(EntityHandle handle) const {
		if (!handle.IsValid())
			return false;

		const u32 index = handle.index;
		if (index >= _alive.size()) return false;
		return _alive[index] && _generations[index] == handle.generation;
	}

	EntityHandle EntityFactory::GetHandleAt(usize index) {
		if (index >= _alive.size() || !_alive[index])
			return EntityHandle();
		return EntityHandle{ (u32)index, _generations[index] };
	}

	Signature* EntityFactory::GetSignatureOf(EntityHandle handle) {
		const u32 index = handle.index;
		if (!IsValid(handle) || index >= _signatures.size())
			return nullptr;

		return &_signatures[index];
	}

	const Signature* EntityFactory::GetSignatureOf(EntityHandle handle) const {
		const u32 index = handle.index;
		if (!IsValid(handle) || index >= _signatures.size())
			return nullptr;

		return &_signatures[index];
	}
}