#pragma once
#include <Runtime/Definitions/Types.h>
#include <string>

struct GLFWwindow;
namespace Tourqe::Engine {
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
		static void GLFWResizeEvent(GLFWwindow* window, int width, int height);

		WindowDesc _desc;
		GLFWwindow* _handle = nullptr;
	};
}