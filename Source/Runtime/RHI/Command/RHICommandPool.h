#pragma once

#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class CommandBuffer;
	struct CommandBufferDesc;

	class CommandPool : public Object {
	public:
		using Object::Object;
		virtual ~CommandPool() = default;

		virtual CommandBuffer* AllocateCommandBuffer(const CommandBufferDesc& desc) = 0;
		virtual void FreeCommandBuffer(CommandBuffer* commandBuffer) = 0;

		virtual void Reset() = 0;
	};
}