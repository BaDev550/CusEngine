#pragma once

#include <Runtime/RHI/ImGui/ImGuiContext.h>
#include <Runtime/PAL/Window/Window.h>
#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanImGuiContext final : public ImGuiContext {
	public:
		VulkanImGuiContext(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, Runtime::RHI::Queue* queue, Runtime::PAL::WindowHandle handle);
		~VulkanImGuiContext();

		virtual void Begin() const final;
		virtual void Flush(Runtime::RHI::CommandBuffer* cmd) const final;
	private:
		VkDevice _owningDevice;
		VkDescriptorPool _imguiDescriptorPool;
	};
}