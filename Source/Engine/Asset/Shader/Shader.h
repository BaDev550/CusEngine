#pragma once
#include <Engine/Asset/Asset.h>
#include <Runtime/RHI/Shader/ShaderDesc.h>

namespace CusEngine {
	class ENGINE_API Shader final : public Asset {
		REFLECT_CLASS()
	public:
		Shader(const RHI::ShaderDesc& desc) : _desc(desc) {};

		[[nodiscard]] RHI::ShaderDesc GetDesc() const { return _desc; }
	private:
		RHI::ShaderDesc _desc;
	};
}