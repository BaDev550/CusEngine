#pragma once
#include <Runtime/Definitions/Types.h>
#include <string>

namespace Runtime::PAL {
	using PALInstance = uptr;
	using WindowHandle = uptr;

	struct WindowDesc {
		u32 width = 800;
		u32 height = 800;
		std::string title = "Window";

		b8 minimized = false;
	};

	class Window {
	public:
		virtual ~Window() = default;

		virtual b8 ShouldClose() const = 0;
		virtual void PollEvents() = 0;
		[[nodiscard]] virtual WindowHandle GetHandle() const = 0;
		[[nodiscard]] virtual PALInstance GetInstance() const = 0;
		[[nodiscard]] virtual const u32 GetWidth() const = 0;
		[[nodiscard]] virtual const u32 GetHeight() const = 0;
		[[nodiscard]] virtual const b8 IsMinimized() const = 0;
	};

	Window* CreateNewWindow(const WindowDesc& desc);
}