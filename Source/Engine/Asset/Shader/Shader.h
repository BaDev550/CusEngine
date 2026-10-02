#pragma once
#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Shader/ShaderDesc.h>

namespace CusEngine {
	class ENGINE_API Shader final : public Asset {
		REFLECT_CLASS()
	public:
		Shader(const Runtime::RHI::ShaderDesc& desc) : _desc(desc) {};

		[[nodiscard]] Runtime::RHI::ShaderDesc& GetDesc() { return _desc; }
	private:
		Runtime::RHI::ShaderDesc _desc;
	};
}