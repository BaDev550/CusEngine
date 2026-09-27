#include "Texture2DStreamer.h"

#include <Engine/Asset/Asset.h>
#include <Engine/Core/Profiler.h>
#include <Runtime/IO/FileBuffer.h>
#include <filesystem>
#include <fstream>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Engine/Asset/Texture/Texture2D.h>

#include <Engine/Renderer/RenderSubsystem.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Image/RHIImage.h>

namespace CusEngine {
    Result Texture2DStreamer::Cook(AssetSource& source) {
        if (source.sourcePath.empty()) {
            return Result("No source file provided!");
        }
        
        FileBuffer textureBuffer;
        int width, height, comp;
        std::vector<u8> rawData;
        std::vector<u8> compressedData;
        textureBuffer.Stream(rawData, source.sourcePath);
        u8* rawImageData = stbi_load_from_memory(rawData.data(), (rawData.size() * sizeof(u8)), &width, &height, &comp, 4);
        
        CompressImageToBC3(rawImageData, width, height, compressedData);

        stbi_image_free(rawImageData);

        AssetHeader header{};
        header.magic = 0x54455854;
        header.width = width;
        header.height = height;
        header.id = source.id;
        header.metaSize = sizeof(AssetHeader);
        std::strcpy(header.typeName, source.type.c_str());
        header.dataSize = (compressedData.size() * sizeof(u8));
        header.dataOffset = sizeof(AssetHeader);

        textureBuffer.Write(source.cookedPath, &header, sizeof(AssetHeader), FileWritingMethod::Binary);
        textureBuffer.Write(source.cookedPath, compressedData.data(), header.dataSize, FileWritingMethod::Binary, header.dataOffset);
        Logger::Info("Texture2DCooker", "Texture cooked to loc: {}", std::filesystem::absolute(source.cookedPath).string());

        return Result();
    }

    Asset* Texture2DStreamer::Import(AssetSource& source) {
        FileBuffer textureBuffer;
        std::vector<u8> fulldata = textureBuffer.Read(source.cookedPath);

        u32 magic;
        std::memcpy(&magic, fulldata.data(), sizeof(u32));
        if (magic != 0x54455854) {
            Logger::Error("Texture2DImporter", "Not a valid texture asset");
            return nullptr;
        }

        AssetHeader header;
        std::memcpy(&header, fulldata.data(), sizeof(AssetHeader));
        std::vector<u8> imageData(fulldata.begin() + sizeof(AssetHeader), fulldata.end());

        Logger::Info("Texture2DImporter", "Imported Texture info: \n type:{}\n datasize:{}\n dataoffset:{}\n UUID:{}\n imageDataSize:{}\n imageWidth:{}\n imageHeight:{}",
            header.typeName,
            header.dataSize,
            header.dataOffset,
            header.id.Str(),
            imageData.size(),
            header.width,
            header.height);

        Texture2D* texture = Mem::Allocator::Construct<Texture2D>(header.width, header.height);
        {
            auto* renderSubsystem = Engine::Get()->GetSubsystem<RenderSubsystem>();
            auto* rhi_context = renderSubsystem->GetContext();
            auto* rhi_commands = renderSubsystem->GetCommands();

            rhi_commands->Submit([=]() {
                RHI::BufferDesc desc{};
                desc.usage = RHI::BufferUsage::TransferSrc;
                desc.memoryUsage = RHI::MemoryUsage::CPUToGPU;
                desc.allocationFlags = RHI::AllocationFlagBits::HostAccessSequentialWrite | RHI::AllocationFlagBits::CreateMapped;
                desc.size = header.dataSize;
                RHI::Buffer* stagingBuffer = rhi_context->CreateBuffer(desc);
                stagingBuffer->Write(imageData.data());
                rhi_commands->Track(stagingBuffer);

                rhi_commands->CopyBufferToImage(stagingBuffer, texture->_image, RHI::ImageLayout::TransferDst, header.width, header.height);
                });
        }

        return texture;
    }

    bool Texture2DStreamer::CompressImageToBC3(u8* rawData, u32 width, u32 height, std::vector<u8>& compressedImage) {
        compressedImage.clear();

        BEGIN_SCOPE(Texture2DNVTT);
        nvtt::Surface surface;
        if (!surface.setImage(nvtt::InputFormat_BGRA_8UB, width, height, 1, rawData)) {
            Logger::Error("Texture2DNVTT", "NVTT failed to set image data");
            return false;
        }
        surface.swizzle(2, 1, 0, 3);

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

#if 0
        // TEMP load texture of testing
        {
#if 1 // method 1 idk ms vise it is good but mio is more optimized but life-time trash version of it
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
#if 0 // good but life-cycle of scope not optimal for texture importing to GPU
            FileBuffer textureBuffer;
            std::vector<u8> data;
            textureBuffer.Stream(data, targetFile.string());

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
#endif
}