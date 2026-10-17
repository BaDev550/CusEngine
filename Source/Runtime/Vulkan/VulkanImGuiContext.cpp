#include "VulkanImGuiContext.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_win32.h>

namespace Runtime::RHI {
	VulkanImGuiContext::VulkanImGuiContext(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, Runtime::RHI::Queue* queue, Runtime::PAL::WindowHandle handle)
	: _owningDevice(device) 
	{
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(device, &pool_info, nullptr, &_imguiDescriptorPool); // add check

		auto* context = ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;
		io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::SetCurrentContext(context);
		ImGui_ImplWin32_Init(reinterpret_cast<void*>(handle));

		VulkanQueue* vkQueue = static_cast<VulkanQueue*>(queue);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.ApiVersion = VK_API_VERSION_1_3;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = device;
		init_info.Queue = vkQueue->GetVkQueue();
		init_info.QueueFamily = vkQueue->GetQueueFamilyIndex();
		init_info.DescriptorPool = _imguiDescriptorPool;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.UseDynamicRendering = true;

		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		VkPipelineRenderingCreateInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		info.colorAttachmentCount = 1;
		info.pColorAttachmentFormats = &format;
		info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = info;

		ImGui_ImplVulkan_Init(&init_info);
		Logger::Info("VulkanImGuiContext", "ImGui initialized");
	}

	VulkanImGuiContext::~VulkanImGuiContext() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplWin32_Shutdown();
		vkDestroyDescriptorPool(_owningDevice, _imguiDescriptorPool, nullptr);
		ImGui::DestroyContext();
	}

	void VulkanImGuiContext::Begin() const {
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();
	}

	void VulkanImGuiContext::Flush(Runtime::RHI::CommandBuffer* cmd) const {
		VulkanCommandBuffer* vkCmd = static_cast<VulkanCommandBuffer*>(cmd);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vkCmd->GetVkCommandBuffer());
	}
}