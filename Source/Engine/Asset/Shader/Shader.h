#pragma once
#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Shader/ShaderDesc.h>

namespace CusEngine {
	CCLASS()
	class ENGINE_API Shader final : public Asset {
		GENERATE_CLASS(Shader)
	public:
		Shader() = default;
		Shader(const Runtime::RHI::ShaderDesc& desc) : _desc(desc) {};

		[[nodiscard]] Runtime::RHI::ShaderDesc& GetDesc() { return _desc; }
	private:
		Runtime::RHI::ShaderDesc _desc;
	};
}