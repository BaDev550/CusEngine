#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Descriptor/DescriptorType.h>
#include <Runtime/RHI/Shader/ShaderStage.h>
#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	struct DescriptorBindingDesc {
		u32 binding = 0;
		u32 count = 1;
		DescriptorType type = DescriptorType::SampledImage;
		ShaderStage stage = ShaderStage::All;

		b8 partiallyBound = false;
		b8 updateAfterBind = false;
	};
	
	class DescriptorSetLayout : public Object {
	public:
		[[nodiscard]] virtual std::vector<DescriptorBindingDesc>& GetBindings() = 0;
	};
}