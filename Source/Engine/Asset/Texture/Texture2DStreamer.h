#pragma once

#include <Engine/Asset/AssetStreamer.h>
#include <Engine/Asset/Texture/Texture2D.h>
#include <nvtt/nvtt.h>

namespace CusEngine {
    struct NvttVectorOutputHandler : public nvtt::OutputHandler {
        std::vector<u8>& _buffer;

        NvttVectorOutputHandler(std::vector<u8>& buffer) : _buffer(buffer) {}

        virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel) override {
            _buffer.reserve(_buffer.size() + size);
        }

        virtual bool writeData(const void* data, int size) override {
            const u8* src = static_cast<const u8*>(data);
            _buffer.insert(_buffer.end(), src, src + size);
            return true;
        }

        virtual void endImage() override {}
    };

	class Texture2DStreamer final : public AssetStreamer {
        GENERATE_CLASS(Texture2DStreamer)
	public:
		virtual Runtime::Result Cook(AssetSource& source) override;
        virtual Asset* Import(AssetSource& source) override;

        virtual std::string GetAssetClassName() override { return Texture2D::StaticClassName().data(); };
	private:
		bool CompressImageToBC3(u8* rawData, u32 width, u32 height, std::vector<u8>& compressedImage);
	};
}