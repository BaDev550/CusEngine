#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Descriptor/DescriptorType.h>
#include <Runtime/RHI/Descriptor/DescriptorSetLayout.h>

#include <Runtime/RHI/Object/RHIObject.h>

#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>

namespace Runtime::RHI {
	class DescriptorSet : public Object {
	public:
		virtual ~DescriptorSet() = default;

		virtual void WriteImage(u32 binding, u32 arrayIndex, DescriptorType type, Image* image) = 0;
		virtual void WriteBuffer(u32 binding, u32 arrayIndex, DescriptorType type, Buffer* buffer) = 0;

		[[nodiscard]] virtual const DescriptorSetLayout* GetLayout() const = 0;
	};
}