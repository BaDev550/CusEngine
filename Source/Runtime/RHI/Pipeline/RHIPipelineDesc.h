#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/RHI/Pipeline/RHIVertexAttributeDesc.h>
#include <Runtime/RHI/Shader/ShaderDesc.h>
#include <Runtime/RHI/Pipeline/RHIPushConstantRange.h>
#include <vector>

namespace Runtime::RHI {
	struct PipelineDesc {
		ShaderDesc* vertexShader = nullptr;
		ShaderDesc* fragmentShader = nullptr;
		ShaderDesc* computeShader = nullptr;

		bool depthTest = true;
		bool blending = true;
		VertexInputDesc inputDesc;

		std::vector<PushConstantRange> pushConstantRanges;

		std::vector<Format> colorFormats;
		Format depthFormat;
	};
}