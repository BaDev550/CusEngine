#include "SPRIV_ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include <iostream>

namespace Graphics::Compiler {
    static shaderc_shader_kind MapShaderStage(RHI_ShaderStage stage) {
        switch (stage) {
        case RHI_ShaderStage::Vertex:   return shaderc_glsl_vertex_shader;
        case RHI_ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
        case RHI_ShaderStage::Compute:  return shaderc_glsl_compute_shader;
        }
        return shaderc_glsl_infer_from_source;
    }

    RHI_ShaderByteCode SPRIV_Compiler::CompileGLSL(std::string_view sourceCode, RHI_ShaderStage stage, std::string_view sourceFileName) {
        RHI_ShaderByteCode result{};
        result.Stage = stage;

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

        shaderc_shader_kind kind = MapShaderStage(stage);

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
            sourceCode.data(),
            sourceCode.size(),
            kind,
            sourceFileName.data(),
            options
        );

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            return result;
        }

        const uint32_t* begin = reinterpret_cast<const uint32_t*>(module.cbegin());
        const uint32_t* end = reinterpret_cast<const uint32_t*>(module.cend());
        result.spriv.assign(begin, end);

        return result;
    }
}