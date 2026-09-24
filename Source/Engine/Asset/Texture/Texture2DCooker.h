#pragma once

#include <Engine/Asset/AssetCooker.h>
#include <nvtt/nvtt.h>

namespace CusEngine {
    struct NvttVectorOutputHandler : public nvtt::OutputHandler {
        std::vector<u8>& m_buffer;

        NvttVectorOutputHandler(std::vector<u8>& buffer) : m_buffer(buffer) {}

        virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel) override {
            m_buffer.reserve(m_buffer.size() + size);
        }

        virtual bool writeData(const void* data, int size) override {
            const u8* src = static_cast<const u8*>(data);
            m_buffer.insert(m_buffer.end(), src, src + size);
            return true;
        }

        virtual void endImage() override {}
    };

	class Texture2DCooker final : AssetCooker {
	public:
		virtual Result Cook(AssetSource& source) override;
	private:
		bool CompressImageToBC3(u8* rawData, u32 width, u32 height, std::vector<u8>& compressedImage);
	};
}