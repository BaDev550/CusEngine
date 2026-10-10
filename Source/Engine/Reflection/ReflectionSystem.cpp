#include "ReflectionSystem.h"

#include <ReflectManifest.h>
#include "ReflectionSystem.h"

namespace Tourqe::Engine {
    ReflectionSystem::ReflectionSystem() {
		GenerateModuleManifestation(&_types); // TEMP

		for (auto& type : _types) {
			Logger::Info("ReflectionSubsystem", "Type {} registered", type.GetName());
		}
    }

    ReflectionSystem::~ReflectionSystem() {
    
    }
}