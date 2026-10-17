#include "WindowsImGuiContext.h"
#include <Runtime/Memory/Memory.h>

#include <imgui.h>
#include <imgui_impl_win32.h>

namespace Runtime::PAL {
	ImGuiPlatform* CreateImGuiForPlatform(WindowHandle handle) {
		WindowsImGui* context = Mem::Allocator::Construct<WindowsImGui>(handle);
		return context;
	}

	WindowsImGui::WindowsImGui(WindowHandle handle) {
		ImGui_ImplWin32_Init(reinterpret_cast<void*>(handle));
	}

	WindowsImGui::~WindowsImGui() {
		ImGui_ImplWin32_Shutdown();
	}

	void WindowsImGui::NewFrame() {
		ImGui_ImplWin32_NewFrame();
	}
}