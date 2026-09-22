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
		header.typeName = StaticMesh::StaticTypeInfo()->Name;
		header.metaSize = sizeof(AssetData);
		header.dataOffset = sizeof(AssetData);
		header.dataSize = data.size();
		
		Logger::Info("StaticMeshCooker", "Reading: magic:{}, size:{}, offset:{}, id:{}", 
			header.magic, 
			header.dataSize, 
			header.dataOffset, 
			header.id.Str()
		);

		return Result();
	}
}