#pragma once

#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class CommandBuffer;
	class Swapchain;
	class Fence;

	class Queue : public Object {
	public:
		using Object::Object;

		virtual ~Queue() = default;

		virtual void Wait() = 0;
		virtual void Submit(CommandBuffer* commandBuffer, const std::vector<Fence*>& waitFences, const std::vector<Fence*>& signalFences) = 0;
		virtual void Present(Swapchain* swapchain, u32 imageIndex, const std::vector<Fence*>& waitFences) = 0;

		[[nodiscard]] virtual u32 GetQueueFamilyIndex() const = 0;
	};
}