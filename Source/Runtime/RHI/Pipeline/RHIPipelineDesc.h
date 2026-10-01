#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/RHI/Pipeline/RHIVertexAttributeDesc.h>
#include <Runtime/RHI/Shader/ShaderDesc.h>
#include <vector>

namespace Runtime::RHI {
	struct PipelineDesc {
		ShaderDesc* vertexShader = nullptr;
		ShaderDesc* fragmentShader = nullptr;
		ShaderDesc* computeShader = nullptr;

		bool depthTest = true;
		bool blending = true;
		VertexInputAttributeDesc attribDesc;

		std::vector<Format> colorFormats;
		Format depthFormat;
	};
}