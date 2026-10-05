#include "Texture2DStreamer.h"

#include <Engine/Asset/Asset.h>
#include <Runtime/Definitions/Profiler.h>
#include <Runtime/IO/FileBuffer.h>
#include <filesystem>
#include <fstream>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Engine/Renderer/RenderSubsystem.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Common/RHIUtils.h>

#include <Runtime/Reflection/TypeRegistry.h>

namespace CusEngine {
    Runtime::Result Texture2DStreamer::Cook(AssetSource& source) {
        if (source.sourcePath.empty()) {
            return Runtime::Result("No source file provided!");
        }
        
        Runtime::IO::FileBuffer textureBuffer;
        int width, height, comp;
        std::vector<u8> rawData;
        std::vector<u8> compressedData;
        textureBuffer.Map(source.sourcePath, rawData);
        u8* rawImageData = stbi_load_from_memory(rawData.data(), (rawData.size() * sizeof(u8)), &width, &height, &comp, 4);
        
        CompressImageToBC3(rawImageData, width, height, compressedData);

        stbi_image_free(rawImageData);

        Runtime::RHI::ImageDesc imageDesc{};
        imageDesc.format = Runtime::RHI::Format::BC3;
        imageDesc.width = width;
        imageDesc.height = height;
        imageDesc.sampler = Runtime::RHI::StaticSampler::NearestClamp;
        imageDesc.view.type = Runtime::RHI::ImageViewType::Image2D;
        imageDesc.usage = Runtime::RHI::ImageUsage::Sampled | Runtime::RHI::ImageUsage::TransferDst;
        imageDesc.tileMode = Runtime::RHI::ImageTileMode::Optimal;

        AssetHeader header{};
        header.magic = TEXTURE2D_MAGIC;
        header.handle = source.handle;
        header.metaSize = sizeof(AssetHeader);
        std::strcpy(header.typeName, source.type.c_str());
        header.dataSize = (compressedData.size() * sizeof(u8));
        header.dataOffset = (sizeof(AssetHeader) + sizeof(Runtime::RHI::ImageDesc));

        textureBuffer.WriteBinary(source.cookedPath, &header, sizeof(AssetHeader));
        textureBuffer.WriteBinary(source.cookedPath, &imageDesc, sizeof(Runtime::RHI::ImageDesc), sizeof(AssetHeader));
        textureBuffer.WriteBinary(source.cookedPath, compressedData.data(), header.dataSize, header.dataOffset);
        Logger::Info("Texture2DCooker", "Texture cooked to loc: {}", std::filesystem::absolute(source.cookedPath).string());

        return Runtime::Result();
    }

    Asset* Texture2DStreamer::Import(AssetSource& source) {
        Runtime::IO::FileBuffer textureBuffer;
        std::vector<u8> fulldata = textureBuffer.ReadBinary(source.cookedPath);

        u32 magic;
        std::memcpy(&magic, fulldata.data(), sizeof(u32));
        if (magic != TEXTURE2D_MAGIC) {
            Logger::Error("Texture2DImporter", "Not a valid texture asset");
            return nullptr;
        }

        AssetHeader header;
        std::memcpy(&header, fulldata.data(), sizeof(AssetHeader));

        Runtime::RHI::ImageDesc imageDesc{};
        std::memcpy(&imageDesc, (fulldata.data() + sizeof(AssetHeader)), sizeof(Runtime::RHI::ImageDesc));
        
        std::vector<u8> imageData((fulldata.begin() + header.dataOffset), fulldata.end());

        Logger::Info("Texture2DImporter", "Imported Texture info: \n type:{}\n datasize:{}\n dataoffset:{}\n Runtime::UUID:{}\n imageDataSize:{}\n imageWidth:{}\n imageHeight:{}\n imageFormat:{}",
            header.typeName,
            header.dataSize,
            header.dataOffset,
            header.handle.Str(),
            imageData.size(),
            imageDesc.width,
            imageDesc.height,
            Runtime::RHI::Utils::FormatToString(imageDesc.format));

        Texture2D* texture = Runtime::Mem::Allocator::Construct<Texture2D>(imageDesc);

        texture->SetAssetState(AssetState::Loading);

        auto* renderSubsystem = Engine::Get()->GetSubsystem<RenderSubsystem>();

        renderSubsystem->Submit([=](Runtime::RHI::CommandBuffer* cmd) {
			RHI::Buffer* stagingBuffer = renderSubsystem->CreateStagingBuffer(header.dataSize);
			stagingBuffer->Write(imageData.data());

			cmd->TransitionImageLayout(texture->_image, Runtime::RHI::ImageLayout::TransferDst);
			cmd->CopyBufferToImage(stagingBuffer, texture->_image, Runtime::RHI::ImageLayout::TransferDst, imageDesc.width, imageDesc.height);
			cmd->TransitionImageLayout(texture->_image, Runtime::RHI::ImageLayout::ShaderReadOnly);
			texture->SetAssetState(AssetState::Ready);
            });

        texture->SetAssetState(AssetState::Loaded);

        return std::move(texture);
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
}

REGISTER_CLASS(Texture2DStreamer, AssetStreamer);