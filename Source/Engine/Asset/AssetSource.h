#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/Definitions/UUID.h>
#include <nlohmann/json.hpp>
#include <string>

namespace CusEngine {
	struct AssetSource {
		Runtime::UUID handle;
		std::string sourcePath;
		std::string cookedPath;
		std::string type;

		AssetSource() = default;
		AssetSource(Runtime::UUID id_, std::string srcPath_, std::string ckPath_, std::string type_) : handle(std::move(id_)), sourcePath(std::move(srcPath_)), cookedPath(std::move(ckPath_)), type(std::move(type_)) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(AssetSource, handle, sourcePath, cookedPath, type);
	};
}