#pragma once
#include <Engine/Core/Core.h>

struct GLFWwindow;

namespace CusEngine::RHI {
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

		GLFWwindow* windowHandle = nullptr;
	};
}