#include "Texture2DCooker.h"

#include <Engine/Asset/Asset.h>
#include <Engine/Core/Profiler.h>
#include <Runtime/IO/FileBuffer.h>
#include <filesystem>
#include <fstream>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
                int width, height, comp;
                std::vector<u8> rawData;
                std::vector<u8> compressedData;
                textureBuffer.ReadMIO(rawData, sourceFile.string());
                u8* rawImageData = stbi_load_from_memory(rawData.data(), (rawData.size() * sizeof(u8)), &width, &height, &comp, 4);

                CompressImageToBC3(rawImageData, width, height, compressedData);

                AssetHeader header{};
                header.magic = 0x54455854;
                header.id = UUID(sourceFile.string());
                header.metaSize = sizeof(AssetHeader);
                std::strcpy(header.typeName, "Texture2D"); // TODO(0x): use RTR
                header.dataSize = (compressedData.size() * sizeof(u8));
                header.dataOffset = sizeof(AssetHeader);

                textureBuffer.Write(targetFile.string(), &header, sizeof(AssetHeader), FileWritingMethod::Text);
                textureBuffer.Write(targetFile.string(), compressedData.data(), header.dataSize, FileWritingMethod::Binary, header.dataOffset);
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
#if 0 // method 1 idk ms vise it is good but mio is more optimized but life-time trash version of it
            FileBuffer textureBuffer;
            std::vector<u8> data = textureBuffer.Read(targetFile.string());

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
#if 0 // easy but ms fucker :p
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
#if 1 // good but life-cycle of scope not optimal for texture importing to GPU
            FileBuffer textureBuffer;
            std::vector<u8> data;
            textureBuffer.ReadMIO(data, targetFile.string());

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
        }

        return Result();
    }

    bool Texture2DCooker::CompressImageToBC3(u8* rawData, u32 width, u32 height, std::vector<u8>& compressedImage) {
        compressedImage.clear();

        BEGIN_SCOPE(Texture2DNVTT);
        nvtt::Surface surface;
        if (!surface.setImage(nvtt::InputFormat_BGRA_8UB, width, height, 1, rawData)) {
            Logger::Error("Texture2DNVTT", "NVTT failed to set image data");
            return false;
        }

        nvtt::CompressionOptions compOptions;
        compOptions.setFormat(nvtt::Format_BC3);
        compOptions.setQuality(nvtt::Quality_Normal);

        NvttVectorOutputHandler outputHandler(compressedImage);
        nvtt::OutputOptions outputOptions;
        outputOptions.setOutputHandler(&outputHandler);
        outputOptions.setOutputHeader(false);

        nvtt::Context context;
        context.enableCudaAcceleration(true);

        bool result = context.compress(surface, 0, 0, compOptions, outputOptions);

        if (result) {
            Logger::Info("Texture2DNVTT", "Image compressed");
            return true;
        }
        END_SCOPE(Texture2DNVTT);

        Logger::Error("Texture2DNVTT", "NVTT compression failed");
        return false;
    }
}