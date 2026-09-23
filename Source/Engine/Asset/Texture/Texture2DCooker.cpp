#include "Texture2DCooker.h"

#include <Engine/Asset/Asset.h>
#include <Runtime/IO/FileBuffer.h>
#include <filesystem>
#include <fstream>
#include <thread>

namespace CusEngine {
    Result Texture2DCooker::Cook(AssetSource& source) {
        if (source.filePath.empty()) {
            return Result("No source file provided!");
        }
        std::filesystem::path sourceFile = source.filePath;
        std::filesystem::path targetFile = std::filesystem::path(source.filePath).replace_extension(ASSET_EXTENSION);
        {
            if (!std::filesystem::exists(targetFile)) { // TEMP

                FileBuffer textureBuffer;
                std::vector<u64> data = textureBuffer.Read(sourceFile.string());

                AssetHeader header{};
                header.magic = 0x54455854;
                header.id = UUID(sourceFile.string());
                header.metaSize = sizeof(AssetHeader);
                std::strcpy(header.typeName, "Texture2D");
                header.dataSize = (data.size() * sizeof(u64));
                header.dataOffset = sizeof(AssetHeader);

                textureBuffer.Write(targetFile.string(), &header, sizeof(AssetHeader), FileWritingMethod::Text);
                textureBuffer.Write(targetFile.string(), data.data(), header.dataSize, FileWritingMethod::Binary, header.dataOffset);
                Logger::Info("Texture2DCooker", "Texture cooked to loc: {}", std::filesystem::absolute(targetFile).string());
                Logger::Info("Texture2DCooker", "Cooked Texture info: \n type:{}\n datasize:{}\n dataoffset:{}\n UUID:{}",
                    header.typeName,
                    header.dataSize,
                    header.dataOffset,
                    header.id.Str());
            }
        }

        // TEMP load texture of testing
        {
#if 1
            FileBuffer textureBuffer;
            std::vector<u64> data = textureBuffer.Read(targetFile.string());

            u32 magic;
            std::memcpy(&magic, data.data(), sizeof(u32));
            if (magic != 0x54455854) {
                Logger::Error("Texture2DImporter", "Not a valid texture asset");
                return Result("");
            }

            AssetHeader header;
            std::memcpy(&header, data.data(), sizeof(AssetHeader));

            Logger::Info("Texture2DImporter", "Imported Texture info: \n type:{}\n datasize:{}\n dataoffset:{}\n UUID:{}",
                header.typeName,
                header.dataSize,
                header.dataOffset,
                header.id.Str());
#endif
#if 0
            std::fstream stream(targetFile.string(), std::ios_base::binary | std::ios_base::in);
            if (!stream.is_open()) {
                Logger::Error("Texture2DImporter", "Failed to open file");
                return Result("");
            }

            u32 magic{};
            stream.read(reinterpret_cast<char*>(&magic), sizeof(u32));
            if (magic != 0x54455854) {
                Logger::Error("Texture2DImporter", "Not a valid texture asset");
                return Result("");
            }

            AssetHeader header{};
            stream.seekg(stream.beg);
            stream.read(reinterpret_cast<char*>(&header), sizeof(AssetHeader));

            Logger::Info("Texture2DImporter", "Imported Texture info: \n type:{}\n datasize:{}\n dataoffset:{}\n UUID:{}",
                header.typeName,
                header.dataSize,
                header.dataOffset,
                header.id.Str());
            stream.close(); 
#endif
        }

        return Result();
    }
}