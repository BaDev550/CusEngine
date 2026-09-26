#pragma once
#include <Engine/Core/Core.h>
#include <iostream>
#include <random>
#include <sstream>

#include <nlohmann/json.hpp>

namespace CusEngine {
	struct ENGINE_API UUID {
	public:
		UUID() {
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<u64> dis;
			_uuid = dis(gen);
		}
		UUID(u64 uuid) : _uuid(uuid) {}
		UUID(std::string_view uuidStr) { _uuid = std::hash<std::string>{}(uuidStr.data()); }

		std::string Str() const {
			return std::to_string(_uuid);
		}

		operator u64() const { return _uuid; }
	private:
		u64 _uuid;
	};

	inline void to_json(nlohmann::json& j, const UUID& uuid) { j = static_cast<u64>(uuid); }
	inline void from_json(const nlohmann::json& j, UUID& uuid) { uuid = UUID(j.get<u64>()); }
}

namespace std {
	template<>
	struct hash<CusEngine::UUID> {
		size_t operator()(const CusEngine::UUID& uuid) const {
			return hash<u64>()(static_cast<uint64_t>(uuid));
		}
	};
}