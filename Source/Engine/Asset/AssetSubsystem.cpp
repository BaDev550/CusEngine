#include "AssetSubsystem.h"
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Runtime/IO/FileBuffer.h>
#include <Engine/Core/Memory.h>

#include <Engine/Asset/Texture/Texture2DCooker.h>
#include <Engine/Asset/Asset.h>

namespace CusEngine {
	Result AssetSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		_assetCookerLookupTable["Texture2D"] = _assetCookers.size();
		_assetCookers.push_back(Mem::Allocator::Construct<Texture2DCooker>());

		return Result();
	}

	void AssetSubsystem::OnDestroy() {
	
	}

	void AssetSubsystem::Load(std::string_view path) {
		FileBuffer textureBuffer;
		std::vector<u8> data;
		textureBuffer.Stream(data, path.data());

		AssetHeader header;
		if (std::memcpy(&header, data.data(), sizeof(AssetHeader))) {
			Logger::Info("AssetSubystem", "Reading asset file");
			
			auto* cooker = _assetCookers[_assetCookerLookupTable[header.typeName]];
			if (cooker) {
				AssetSource source;
				source.filePath = path.data();

				Asset* ast = cooker->Cook(source);
				_assets[ast->GetAssetID()] = ast;
				_assetSources[ast->GetAssetID()] = source;
				Logger::Info("AssetSubsystem", "Asset loaded");
			}
			else {
				Logger::Error("AssetSubsystem", "Failed to load asset no cooker: {}", header.typeName);
			}
		}
		else {
			Logger::Error("AssetSubsystem", "Failed to load asset not a valid asset");
		}	
	}

	void AssetSubsystem::Unload(UUID id) {

	}

	bool AssetSubsystem::AssetLoaded(UUID id) {

	}

	void AssetSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<RenderSubsystem>(DependencyOrder::Before);
		graph.Require<PluginSubsystem>(DependencyOrder::After);
	}
}