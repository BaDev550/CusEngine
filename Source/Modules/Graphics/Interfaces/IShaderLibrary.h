#pragma once

namespace Graphics {
	class IShaderLibrary {
	public:
		virtual ~IShaderLibrary() = default;

		virtual void LoadAndCompileShader() = 0;
		virtual void LoadShader() = 0;
		virtual void CompileShaderSync() = 0;
		virtual void CompileShaderAsync() = 0;

		virtual void* GetShader() = 0;
		virtual void** GetShaders() = 0;
	};
}