#include <Engine/Asset/StaticMesh/StaticMeshCooker.h>
#include <Engine/Asset/StaticMesh/StaticMesh.h>
#include <Engine/Asset/Asset.h>
#include <Runtime/IO/FileBuffer.h>

namespace CusEngine {
	Result StaticMeshCooker::Cook(AssetSource& source) {
		std::string sourcePath = source.filePath;
		
		std::vector<u64> data;
		{
			FileBuffer buffer(sourcePath);
			data = buffer.Read();
		}
		AssetData header;
		header.magic = 0x4D4F444C; // MODL
		header.id = UUID();
		header.metaSize = sizeof(AssetData);
		header.dataOffset = sizeof(AssetData);
		header.dataSize = data.size();
		std::strcpy(header.typeName, StaticMesh::StaticTypeName().data());
		std::string name = StaticMesh::StaticTypeName().data();

		Logger::Info("StaticMeshCooker", "Reading: magic:{}, size:{}, offset:{}, id:{} type:{}", 
			header.magic, 
			header.dataSize, 
			header.dataOffset, 
			header.id.Str(),
			header.typeName
		);

		return Result();
	}
}