#pragma once

#include <Runtime/Definitions/UUID.h>
#include <chrono>
#include <list>
#include <string_view>

namespace Runtime {
	class Profile final {
	public:
		Profile(std::string_view name) : _name(name) { // TODO(0x): make a registry class to register the duration to a name and destroy it :p
			_start = std::chrono::high_resolution_clock::now();
		}

		~Profile() {
			auto end = std::chrono::high_resolution_clock::now();
			float durationMS = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - _start).count();
			Logger::Info(_name, "Took {:.2f}/ms", durationMS);
		}
	private:
		std::string _name;
		std::chrono::steady_clock::time_point _start;
	};

#define BEGIN_SCOPE(name) { \
	Profile auto_scope_##name{ #name };

#define END_SCOPE(name) }
}