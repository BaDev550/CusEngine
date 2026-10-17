#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/PAL/Window/Window.h>

#include <Windows.h>

namespace Runtime::PAL {
	class WindowsWindow final : public Window { // FIXME: also needs an event manager
	public:
		WindowsWindow(const WindowDesc& desc);
		~WindowsWindow();

		[[nodiscard]] virtual const Platform GetPlatform() const final { return Platform::Windows; }

		virtual b8 ShouldClose() const final;
		virtual void PollEvents() final;
		[[nodiscard]] virtual WindowHandle GetHandle() const final;
		[[nodiscard]] virtual PALInstance GetInstance() const final;
		[[nodiscard]] virtual const u32 GetWidth() const final { return _desc.width; }
		[[nodiscard]] virtual const u32 GetHeight() const final { return _desc.height; }
		[[nodiscard]] virtual const b8 IsMinimized() const final { return _desc.minimized; }
	private:
		WindowDesc _desc;

		HWND _handle;
		HINSTANCE _instance;
		b8 _running = false;
		
		const char* CLASSNAME = "PALWindowsWindowClass";
	};
}