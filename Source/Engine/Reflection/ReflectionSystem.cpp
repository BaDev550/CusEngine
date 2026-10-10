#include "ReflectionSystem.h"

#include <ReflectManifest.h>
#include "ReflectionSystem.h"

namespace Tourqe::Engine {
    ReflectionSystem::ReflectionSystem() {
		GenerateModuleManifestation(&_types); // TEMP

		for (auto& type : _types) {
            _lookupTable[type.GetTypeIndex()] = &type;

			Logger::Info("ReflectionSubsystem", "Type {} registered", type.GetName());
		}
    }

    ReflectionSystem::~ReflectionSystem() {
    
    }
}