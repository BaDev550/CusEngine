#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/UUID.h>
#include <string>

#include <nlohmann/json.hpp>

namespace CusEngine {
	struct AssetSource {
		UUID id;
		std::string sourcePath;
		std::string cookedPath;
		std::string type;

		AssetSource() = default;
		AssetSource(UUID id_, std::string srcPath_, std::string ckPath_, std::string type_) : id(std::move(id_)), sourcePath(std::move(srcPath_)), cookedPath(std::move(ckPath_)), type(std::move(type_)) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(AssetSource, id, sourcePath, cookedPath, type);
	};
}