#pragma once
#include "Core/Types.h"
#include "Graphics/RHI/RHI_Swapchain.h"
#include <string>

struct GLFWwindow;
namespace CusEngine {
	struct WindowDesc {
		u32 width = 800;
		u32 height = 800;
		std::string title = "Window";
	};

	class Window final {
	public:
		Window(const WindowDesc& desc);
		~Window();

		bool ShouldClose() const;
		void PollEvents() const;
		[[nodiscard]] GLFWwindow* GetHandle() const { return _handle; }
		[[nodiscard]] const u32 GetWidth() const { return _desc.width; }
		[[nodiscard]] const u32 GetHeight() const { return _desc.height; }

		RHI::RenderContext* GetRenderContext() const { return _renderContext; }
		RHI::Swapchain* GetSwapchain() const { return _swapchain; }
	private:
		WindowDesc _desc;
		GLFWwindow* _handle = nullptr;

		RHI::RenderContext* _renderContext = nullptr;
		RHI::Swapchain* _swapchain = nullptr;
	};
}