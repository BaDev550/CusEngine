#pragma once

#include <Runtime/PAL/ImGuiPlatform.h>

namespace Runtime::PAL {
	class WindowsImGui final : public ImGuiPlatform {
	public:
		WindowsImGui(WindowHandle handle);
		~WindowsImGui();

		virtual void NewFrame() override;
	};
}