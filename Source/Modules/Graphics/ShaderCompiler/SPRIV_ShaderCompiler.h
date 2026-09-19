#pragma once

#include "Graphics/RHI/RHI.h"

namespace Graphics::Compiler {
	class SPRIV_Compiler final {
	public:
		static RHI_ShaderByteCode CompileGLSL(std::string_view sourceCode, RHI_ShaderStage stage, std::string_view sourceFileName);
	};
}