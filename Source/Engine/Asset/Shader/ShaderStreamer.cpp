#include "ShaderStreamer.h"

#include <Engine/Asset/Asset.h>
#include <Engine/Core/Profiler.h>
#include <Runtime/IO/FileBuffer.h>
#include <fstream>

#include <Engine/Asset/Shader/Shader.h>

namespace CusEngine {
	Result ShaderStreamer::Cook(AssetSource& source) { // TEMP!!!!! FIX IT FUCKED UP RHI
		if (source.sourcePath.empty()) {
			return Result("No source file provided!");
		}
		std::string afterDot = std::filesystem::path(source.sourcePath).extension().string();

		shaderc_shader_kind kind;
		if (afterDot == ".vert") kind = shaderc_vertex_shader;
		else if (afterDot == ".frag") kind = shaderc_fragment_shader;
		else if (afterDot == ".geo") kind = shaderc_geometry_shader;
		else if (afterDot == ".comp") kind = shaderc_compute_shader;

		FileBuffer shaderBuffer;
		std::vector<u8> rawData;
		std::vector<u8> compiledData;
		shaderBuffer.Stream(rawData, source.sourcePath);
		std::string sourceStr(rawData.begin(), rawData.end());

		if (sourceStr.empty()) {
			return Result("Failed to open file");
		}

		Logger::Info("ShaderStreamer", "Compiling shader: {}", source.sourcePath);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetIncluder(std::make_unique<FileIncluder>());
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		options.SetTargetSpirv(shaderc_spirv_version_1_5);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		shaderc::CompilationResult result = compiler.CompileGlslToSpv(sourceStr, kind, source.sourcePath.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			Logger::Error("ShaderStreamer", "{}", result.GetErrorMessage());
			return Result("Failed to compile shader");
		}
		compiledData.assign(result.cbegin(), result.cend());

		AssetHeader header{};
		header.magic = 0x53484452;
		header.id = source.id;
		header.metaSize = sizeof(AssetHeader);
		std::strcpy(header.typeName, source.type.c_str());
		header.dataSize = (compiledData.size() * sizeof(u8));
		header.dataOffset = sizeof(AssetHeader);

		shaderBuffer.Write(source.cookedPath, &header, sizeof(AssetHeader), FileWritingMethod::Binary);
		shaderBuffer.Write(source.cookedPath, compiledData.data(), header.dataSize, FileWritingMethod::Binary, header.dataOffset);
		Logger::Info("ShaderStreamer", "Cooked to loc: {}", std::filesystem::absolute(source.cookedPath).string());

		return Result();
	}

    Asset* ShaderStreamer::Import(AssetSource& source) {

        return nullptr;
    }

	shaderc_include_result* FileIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) {
		auto result = new shaderc_include_result;

		std::ifstream file(requested_source);

		if (!file.is_open()) {
			result->source_name = "";
			result->content = "File not found";
			return result;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		std::string* contentStr = new std::string(buffer.str());
		std::string* nameStr = new std::string(requested_source);

		result->source_name = nameStr->c_str();
		result->source_name_length = nameStr->length();
		result->content = contentStr->c_str();
		result->content_length = contentStr->length();

		result->user_data = new std::pair<std::string*, std::string*>(contentStr, nameStr);
		return result;
	}
}