#include "OpenGLImage.h"

#include <Core/Logger.h>
#include <Graphics/RHI/RHI_Utils.h>

#include "OpenGLRenderCommands.h"
#include "OpenGLUtils.h"

namespace CusEngine::RHI {
	OpenGL_Image::OpenGL_Image(RenderCommands* commands, const ImageDesc& desc) : Image(commands), _desc(desc) {
		Logger::Info("OpenGL Image", "Created image with width: {}, height: {}, format: {}, usage: {}, tile mode: {}",
			_desc.width, _desc.height, Utils::FormatToString(_desc.format), Utils::ImageUsageToString(_desc.usage), Utils::ImageTileModeToString(_desc.tileMode));
	}

	OpenGL_Image::~OpenGL_Image() {
		Logger::Info("OpenGL Image", "Destroyed Vulkan image");
	}
}