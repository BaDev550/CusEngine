#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/PAL/Window/Window.h>

namespace Runtime::PAL {
	class ImGuiPlatform {
	public:
		virtual ~ImGuiPlatform() = default;

		virtual void NewFrame() = 0;
	};

	ImGuiPlatform* CreateImGuiForPlatform(WindowHandle handle);
}