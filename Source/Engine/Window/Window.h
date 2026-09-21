#pragma once
#include <Engine/Core/Types.h>
#include <string>

#include <Runtime/RHI/Context/RHIContext.h>
#include <Runtime/RHI/Swapchain/RHISwapchain.h>
#include <Runtime/RHI/Command/RHICommands.h>

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
	private:
		WindowDesc _desc;
		GLFWwindow* _handle = nullptr;

	public:
		RHI::Context* _context = nullptr;
		RHI::Commands* _commands = nullptr;
		RHI::Swapchain* _swapchain = nullptr;
	};
}