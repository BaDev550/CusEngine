#pragma once
#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>

#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetCooker.h>
#include <Engine/Asset/AssetSource.h>
#include <unordered_set>

namespace CusEngine {
	class ENGINE_API AssetSubsystem final : public Subsystem {
	public:
		virtual Result OnCreate(Engine* engine) override;
		virtual void OnDestroy() override;
		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		void Load(std::string_view path);
		void Unload(UUID id);
	private:
		bool AssetLoaded(UUID id);
		
		std::vector<AssetCooker*> _assetCookers;
		std::unordered_map<std::string, u32> _assetCookerLookupTable;

		std::unordered_map<UUID, Asset*> _assets;
		std::unordered_map<UUID, AssetSource> _assetSources;
	};
}