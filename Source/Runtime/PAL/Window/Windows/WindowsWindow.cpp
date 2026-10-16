#include "WindowsWindow.h"

#include <Runtime/Memory/Memory.h>

namespace Runtime::PAL {
	Window* CreateNewWindow(const WindowDesc& desc) {
		return Mem::Allocator::Construct<WindowsWindow>(desc);
	}

	inline WindowDesc* GetWindowDescParam(HWND hwnd) {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (!ptr) return nullptr;
		WindowDesc* desc = reinterpret_cast<WindowDesc*>(ptr);
		return desc;
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		WindowDesc* desc = GetWindowDescParam(hwnd);
		switch (uMsg)
		{
		case WM_CREATE: {
			CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);

			desc = reinterpret_cast<WindowDesc*>(create->lpCreateParams);
			if (!desc) {
				Logger::Fatal("WindowsWindow", "null user pointer data");
				return -1;
			}
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)desc);
			return 0;
		}
		case WM_SIZE: {
			desc->width = LOWORD(lParam);
			desc->height = HIWORD(lParam);

			if (desc->width == 0 || desc->height == 0) desc->minimized = true;
			else desc->minimized = false;

			Logger::Info("WindowsWindow", "Window resized to {}x{}", desc->width, desc->height);
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		default:
			break;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowDesc& desc) : _desc(desc) {
		STARTUPINFO startup{};
		GetStartupInfo(&startup);
		int cmdShow = startup.wShowWindow;

		WNDCLASSEX wc{};
		wc.lpszClassName = TEXT(CLASSNAME);
		wc.lpfnWndProc = WindowProc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = nullptr;
		wc.hInstance = _instance;
		RegisterClassEx(&wc);

		_handle = CreateWindowEx(
			0,
			TEXT(CLASSNAME),
			TEXT(_desc.title.c_str()),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, _desc.width, _desc.height,
			nullptr,
			nullptr,
			_instance,
			&_desc
		);
		if (_handle == nullptr) {
			Logger::Error("WindowsWindow", "Failed to create window");
			return;
		}

		ShowWindow(_handle, 1);
		UpdateWindow(_handle);
		_running = true;
	}

	WindowsWindow::~WindowsWindow() {
		DestroyWindow(_handle);
	}

	b8 WindowsWindow::ShouldClose() const {
		return !_running;
	}

	void WindowsWindow::PollEvents() {
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				_running = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WindowHandle WindowsWindow::GetHandle() const { return reinterpret_cast<WindowHandle>(_handle); }
	PALInstance WindowsWindow::GetInstance() const { return reinterpret_cast<PALInstance>(_instance); }
}