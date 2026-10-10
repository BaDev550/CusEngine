#pragma once
#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>

#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetStreamer.h>
#include <Engine/Asset/AssetSource.h>
#include <unordered_set>

namespace Tourqe::Engine {
	class ENGINE_API AssetSubsystem final : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnDestroy() override;
		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		template<class AssetT> requires(std::is_base_of_v<Asset, AssetT>)
		AssetT* Get(const std::string& path) {
			std::string assetType = AssetT::StaticClassName().data();

			std::filesystem::path sourcePath = path;
			std::filesystem::path targetPath = "cooked" / std::filesystem::path(path).replace_extension(ASSET_EXTENSION);
			Runtime::UUID assetID = Runtime::UUID(sourcePath.string()); // TODO(0x): add a time to hashing so it is not exatcly with same named files!! mem leak

			if (AssetInCache(assetID)) {
				Logger::Info("AssetSubsystem", "Asset in registry loading...");
				return static_cast<AssetT*>(Load(assetID));
			}
			else {
				AssetStreamer* streamer = GetAssetCookerOfType(assetType);

				if (streamer) {
					AssetSource source;
					source.handle = assetID;
					source.type = assetType;
					source.sourcePath = sourcePath.string();
					source.cookedPath = targetPath.string();

					Runtime::Result result = streamer->Cook(source);
					if (result) {
						_cachedAssetSources[source.handle] = source;

						AssetT* ast = static_cast<AssetT*>(Load(source.handle));

						SaveRegistry();
						return ast;
					}
					return nullptr;
				}
			}
			return nullptr;
		}
	private:
		Asset* Load(Runtime::UUID id);
		void Unload(Runtime::UUID id);

		AssetStreamer* GetAssetCookerOfType(const std::string& type);
		bool AssetInCache(Runtime::UUID id);
		bool AssetLoaded(Runtime::UUID id);
		void SaveRegistry();
		void LoadRegistry();
		
		std::unordered_map<Runtime::UUID, AssetSource> _cachedAssetSources;

		std::vector<AssetStreamer*> _assetStreamers;
		std::unordered_map<std::string, u32> _assetStreamerLookupTable;

		std::unordered_map<Runtime::UUID, Asset*> _assets;
	};
}