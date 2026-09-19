#pragma once

#include "Graphics/RHI/RHI.h"

namespace CusEngine::RHI::Compiler {
	class SPRIV_Compiler final {
	public:
		static ShaderByteCode CompileGLSL(std::string_view sourceCode, ShaderStage stage, std::string_view sourceFileName);
	};
}