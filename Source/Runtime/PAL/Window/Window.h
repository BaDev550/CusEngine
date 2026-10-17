#pragma once
#include <Runtime/PAL/Window/WindowDesc.h>
#include <Runtime/Definitions/Types.h>
#include <string>

namespace Runtime::PAL {
	class Window {
	public:
		virtual ~Window() = default;

		[[nodiscard]] virtual const Platform GetPlatform() const = 0;

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