#pragma once
#include <Engine/Core/Core.h>
#include <Runtime/PAL/Window/Window.h>

namespace Runtime::RHI {
	struct ContextDesc {
		struct Features {
			bool dynamicRendering = false;
			bool synchronization2 = false;
			bool timelineSemaphore = false;
			bool bufferDeviceAddress = false;
			bool descriptorIndexing = false;
			bool runtimeDescriptorArray = false;
			bool robustBufferAccess = false;
			bool samplerAnisotropy = false;
			bool rayTracing = false;
			bool meshShader = false;
		} features;
		bool enableValidationLayer = false;

		Runtime::PAL::PALInstance instance;
		Runtime::PAL::WindowHandle windowHandle;
	};
}