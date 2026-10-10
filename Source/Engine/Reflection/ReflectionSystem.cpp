#include "ReflectionSystem.h"

#include <ReflectManifest.h>
#include "ReflectionSystem.h"

namespace Tourqe::Engine {
    ReflectionSystem::ReflectionSystem() {
		GenerateModuleManifestation(&_types); // TEMP

		for (auto& type : _types) {
            _lookupTable[type.GetTypeIndex()] = &type;
            _nameLookupTable[type.GetName()] = &type;

			Logger::Info("ReflectionSubsystem", "Type {} registered", type.GetName());
		}
    }

    ReflectionSystem::~ReflectionSystem() {
    
    }

	Runtime::Reflection::Type* ReflectionSystem::GetType(const std::string& name)
	{
		if (!_nameLookupTable.contains(name)) {
			Logger::Error("ReflectionSystem", "Requested type '{}' is not registered", name);
			return nullptr;
		}
		auto it = _nameLookupTable.find(name);
		return it->second;
	}
}