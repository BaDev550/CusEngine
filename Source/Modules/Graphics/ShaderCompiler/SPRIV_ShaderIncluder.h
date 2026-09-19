#pragma once

#include <shaderc/shaderc.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>

namespace Graphics::Compiler::Utils {
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
    public:
        explicit ShaderIncluder(std::filesystem::path baseDirectory = ".") : _baseDir(std::move(baseDirectory)) {}

        shaderc_include_result* GetInclude(
            const char* requested_source,
            shaderc_include_type type,
            const char* requesting_source,
            size_t include_depth
        ) override {
            namespace fs = std::filesystem;

            fs::path requestingPath(requesting_source);
            fs::path targetPath = requestingPath.parent_path() / requested_source;

            if (!fs::exists(targetPath)) {
                targetPath = _baseDir / requested_source;
            }

            auto container = std::make_unique<IncludeData>();
            container->sourceName = targetPath.string();

            std::ifstream file(targetPath, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                container->content = "Failed to open file: " + container->sourceName;
                return MakeIncludeResult(container.release(), false);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            container->content = buffer.str();

            return MakeIncludeResult(container.release(), true);
        }

        void ReleaseInclude(shaderc_include_result* data) override {
            if (data) {
                delete static_cast<IncludeData*>(data->user_data);
                delete data;
            }
        }

    private:
        struct IncludeData {
            std::string sourceName;
            std::string content;
        };

        shaderc_include_result* MakeIncludeResult(IncludeData* data, bool success) {
            auto* result = new shaderc_include_result();
            result->source_name = data->sourceName.c_str();
            result->source_name_length = data->sourceName.length();

            if (success) {
                result->content = data->content.c_str();
                result->content_length = data->content.length();
            }
            else {
                result->content = "";
                result->content_length = 0;
            }

            result->user_data = data;
            return result;
        }

        std::filesystem::path _baseDir;
    };
}