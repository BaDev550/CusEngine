#include "AssetSubsystem.h"
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Reflection/ReflectionSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Runtime/IO/FileBuffer.h>
#include <Runtime/Memory/Memory.h>

#include <Engine/Asset/Texture/Texture2DStreamer.h>
#include <Engine/Asset/Shader/ShaderStreamer.h>
#include <Engine/Asset/Asset.h>

#include <Engine/Asset/Texture/Texture2D.h>
#include <Engine/Asset/Shader/Shader.h>

#include <nlohmann/json.hpp>
#include <fstream>

namespace CusEngine {
	Runtime::Result AssetSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		auto* reflectSystem = Engine::Get()->GetSubsystem<Reflect::ReflectionSubsystem>();

		//for (auto [id, classType] : reflectSystem->GetClasses()) {
		//	_assetCookerLookupTable[id] = _assetCookers.size();
		//	_assetCookers.push_back(Runtime::Mem::Allocator::Construct<Texture2DCooker>());
		//}

		std::string textureTypeName = Texture2D::StaticClassName().data(); // FIXME(0x): wtf baran
		std::string shaderTypeName = Shader::StaticClassName().data();

		_assetStreamerLookupTable[textureTypeName] = _assetStreamers.size();
		_assetStreamers.push_back(Runtime::Mem::Allocator::Construct<Texture2DStreamer>());

		_assetStreamerLookupTable[shaderTypeName] = _assetStreamers.size();
		_assetStreamers.push_back(Runtime::Mem::Allocator::Construct<ShaderStreamer>());

		LoadRegistry();

		return Runtime::Result();
	}

	void AssetSubsystem::OnDestroy() {
		for (const auto& [id, ast] : _assets) {
			Runtime::Mem::Allocator::Destroy(ast);
		}
		_assets.clear();

		for (const auto& streamer : _assetStreamers) {
			Runtime::Mem::Allocator::Destroy(streamer);
		}
		_assetStreamers.clear();
		_assetStreamerLookupTable.clear();
	}

	Asset* AssetSubsystem::Load(Runtime::UUID id) {
		Logger::Info("AssetSubystem", "Reading asset file");

		AssetSource cachedSource = _cachedAssetSources.find(id)->second;
		AssetStreamer* streamer = GetAssetCookerOfType(cachedSource.type);
		if (streamer) {
			Asset* ast = streamer->Import(cachedSource);
			if (ast) {
				ast->SetAssetStreamer(streamer);
				ast->SetAssetHandle(id);

				_assets[ast->GetAssetHandle()] = ast;

				Logger::Info("AssetSubsystem", "Asset loaded");

				return ast;
			}
		}
		else {
			Logger::Error("AssetSubsystem", "Failed to load asset no cooker: {}", cachedSource.type);
		}
	}

	void AssetSubsystem::Unload(Runtime::UUID id) {

	}

	bool AssetSubsystem::AssetLoaded(Runtime::UUID id) {
		if (_assets.find(id) == _assets.end()) {
			Logger::Warn("AssetSubsystem", "Asset {} is not loaded", id.Str());
			return false;
		}
		return true;
	}

	bool AssetSubsystem::AssetInCache(Runtime::UUID id) {
		if (_cachedAssetSources.find(id) == _cachedAssetSources.end()) {
			Logger::Warn("AssetSubsystem", "Asset {} is not in cache", id.Str());
			return false;
		}
		return true;
	}

	AssetStreamer* AssetSubsystem::GetAssetCookerOfType(const std::string& type) {
		auto it = _assetStreamerLookupTable.find(type);

		if (it == _assetStreamerLookupTable.end()) {
			Logger::Warn("AssetSubsystem", "Asset cooker of type {} is not registered", type);
			return nullptr;
		}
		
		return _assetStreamers[it->second];
	}

	void AssetSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<Reflect::ReflectionSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
		graph.Require<PluginSubsystem>(DependencyOrder::After);
	}

	void AssetSubsystem::SaveRegistry() {
		nlohmann::json j;

		std::vector<nlohmann::json> registry;
		for (const auto& [id, source] : _cachedAssetSources) {
			registry.push_back(source);
		}
		j["assets"] = registry;

		std::ofstream file(AssetRegistryPath.data());
		if (!file.is_open()) return;

		file << j.dump(4);
	}

	void AssetSubsystem::LoadRegistry() {
		std::ifstream file(AssetRegistryPath.data());
		if (!file.is_open()) {
			Logger::Warn("AssetSubsystem", "Failed to find registry file");
			return;
		}

		std::vector<AssetSource> sources;
		try {
			nlohmann::json j;
			file >> j;

			if (j.contains("assets")) {
				sources = j["assets"].get<std::vector<AssetSource>>();

				_cachedAssetSources.clear();
				for (auto& source : sources) {
					Logger::Info("AssetSubsystem", "Asset cached: \n id: {}\n type: {}\n source path: {}\n cooked path: {}", source.handle.Str(), source.type, source.sourcePath, source.cookedPath);
					_cachedAssetSources[source.handle] = std::move(source);
				}
			}
			else {
				sources = j.get<std::vector<AssetSource>>();
			}
		}
		catch (const nlohmann::json::exception& e) {
			Logger::Error("AssetSubsystem", "JSON Deserialization error: {}", e.what());
			return;
		}
	}
}