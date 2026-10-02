#include "ShaderStreamer.h"

#include <Engine/Asset/Asset.h>
#include <Runtime/Definitions/Profiler.h>
#include <Runtime/IO/FileBuffer.h>
#include <fstream>
#include <filesystem>

#include <Engine/Asset/Shader/Shader.h>

namespace CusEngine {
	Runtime::Result ShaderStreamer::Cook(AssetSource& source) { // TEMP!!!!! FIX IT FUCKED UP RHI
		if (source.sourcePath.empty()) {
			return Runtime::Result("No source file provided!");
		}

		std::string extension = std::filesystem::path(source.sourcePath).extension().string();

		shaderc_shader_kind kind;
		if (extension == ".vert")      kind = shaderc_vertex_shader;
		else if (extension == ".frag") kind = shaderc_fragment_shader;
		else if (extension == ".geo")  kind = shaderc_geometry_shader;
		else if (extension == ".comp") kind = shaderc_compute_shader;
		else {
			return Runtime::Result("Unsupported shader extension: " + extension);
		}

		Runtime::IO::FileBuffer shaderBuffer;
		std::vector<u8> rawData;
		shaderBuffer.Map(source.sourcePath, rawData);

		std::string sourceStr(rawData.begin(), rawData.end());

		Logger::Info("ShaderStreamer", "Compiling shader: {}", source.sourcePath);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetIncluder(std::make_unique<FileIncluder>());
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		options.SetTargetSpirv(shaderc_spirv_version_1_5);
		options.SetOptimizationLevel(shaderc_optimization_level_size);

		shaderc::CompilationResult result = compiler.CompileGlslToSpv(sourceStr, kind, source.sourcePath.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			Logger::Error("ShaderStreamer", "{}", result.GetErrorMessage());
			return Runtime::Result("Failed to compile shader");
		}

		std::vector<u32> sprivCode = std::vector<u32>(result.cbegin(), result.cend());
		usize sprivByteSize = (sprivCode.size() * sizeof(u32));

		AssetHeader header{};
		header.magic = SHADER_MAGIC;
		header.handle = source.handle;
		header.metaSize = sizeof(AssetHeader);
		std::strncpy(header.typeName, source.type.c_str(), sizeof(header.typeName) - 1);
		header.dataSize = static_cast<u64>(sprivByteSize);
		header.dataOffset = sizeof(AssetHeader);

		shaderBuffer.WriteBinary(source.cookedPath, &header, header.metaSize);
		shaderBuffer.WriteBinary(source.cookedPath, sprivCode.data(), header.dataSize, header.dataOffset);

		Logger::Info("ShaderStreamer", "Cooked to loc: {}", std::filesystem::absolute(source.cookedPath).string());

		return Runtime::Result();
	}

    Asset* ShaderStreamer::Import(AssetSource& source) {
		Runtime::IO::FileBuffer shaderBuffer;
		std::vector fulldata = shaderBuffer.ReadBinary(source.cookedPath);

		if (fulldata.size() < sizeof(AssetHeader)) {
			Logger::Error("ShaderStreamer", "Corrupted shader file (too small): {}", source.cookedPath);
			return nullptr;
		}

		u32 magic = 0;
		std::memcpy(&magic, fulldata.data(), sizeof(u32));
		if (magic != SHADER_MAGIC) {
			Logger::Error("ShaderStreamer", "Not a valid shader asset: {}", source.cookedPath);
			return nullptr;
		}

		AssetHeader header{};
		std::memcpy(&header, fulldata.data(), sizeof(AssetHeader));

		if (header.dataOffset + header.dataSize > fulldata.size()) {
			Logger::Error("ShaderStreamer", "Shader data bounds exceed file length: {}", source.cookedPath);
			return nullptr;
		}

		Runtime::RHI::ShaderDesc desc{};
		desc.code.resize(header.dataSize / sizeof(u32));
		desc.size = header.dataSize;

		std::memcpy(desc.code.data(), fulldata.data() + header.dataOffset, header.dataSize);

		Shader* shader = Runtime::Mem::Allocator::Construct<Shader>(desc);

		return std::move(shader);
    }

	shaderc_include_result* FileIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) {
		auto result = new shaderc_include_result{};

		std::filesystem::path reqPath(requesting_source);
		std::filesystem::path targetPath = reqPath.parent_path() / requested_source;

		std::ifstream file(std::filesystem::exists(targetPath) ? targetPath : requested_source, std::ios::binary);

		if (!file.is_open()) {
			result->source_name = "";
			result->source_name_length = 0;
			result->content = "File not found";
			result->content_length = 14;
			result->user_data = nullptr;
			return result;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		auto* contentStr = new std::string(buffer.str());
		auto* nameStr = new std::string(requested_source);

		result->source_name = nameStr->c_str();
		result->source_name_length = nameStr->length();
		result->content = contentStr->c_str();
		result->content_length = contentStr->length();

		result->user_data = new std::pair(contentStr, nameStr);
		return result;
	}
}