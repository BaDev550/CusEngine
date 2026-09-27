#pragma once

#include <Engine/Asset/AssetStreamer.h>

#include <shaderc/shaderc.hpp>

namespace CusEngine {
    class ShaderStreamer final : public AssetStreamer {
    public:
        static constexpr std::string_view ShaderCacheFileDir = "ShaderCache";

        virtual Result Cook(AssetSource& source) override;
        virtual Asset* Import(AssetSource& source) override;
    };

	class FileIncluder : public shaderc::CompileOptions::IncluderInterface {
	public:
		shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override;
		void ReleaseInclude(shaderc_include_result* data) override {
			auto extra_data = static_cast<std::pair<std::string*, std::string*>*>(data->user_data);
			delete extra_data->first;
			delete extra_data->second;
			delete extra_data;
			delete data;
		}
	};
}