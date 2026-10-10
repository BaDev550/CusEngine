#pragma once

#include <Runtime/Memory/Memory.h>
#include <Engine/World/Objects/Object.h>
#include <list>

namespace Tourqe::Engine {
	class GarbageCollector final {
	public:
		void DestroyObject(TObject* object) {
			if (object->_objectState == ObjectState::PendingDestroy)
				return;
			_pendingList.push_back(object);
		}

		void Collect() {
			for (auto object : _pendingList) { // fucking O(N)
				if (!object) continue;

				if (object->_objectState == ObjectState::PendingDestroy) {
					Logger::Info("GC", "Collecting '{}'", object->GetObjectID().Str());
					Runtime::Mem::Allocator::Destroy(object);
				}
			}
			_pendingList.clear();
		}
	private:
		std::vector<TObject*> _pendingList;
		// Make an array with generational handle like
		// objectHandle
		//		id
		//		generation
	};
}