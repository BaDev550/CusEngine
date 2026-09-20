#include "OpenGLBuffer.h"
#include "OpenGLRenderCommands.h"
#include "OpenGLUtils.h"

namespace CusEngine::RHI {
	OpenGL_Buffer::OpenGL_Buffer(RenderCommands* commands, const BufferDesc& desc) : Buffer(commands), _desc(desc) {

	}

	OpenGL_Buffer::~OpenGL_Buffer() {

	}

	void OpenGL_Buffer::Write(const void* data, usize size, usize offset) {
		if (size == SIZE_MAX) {
			std::memcpy(_mappedPtr, data, _desc.size);
		}
		else {
			void* oData = ((char*)data + offset);
			std::memcpy(_mappedPtr, oData, size);
		}
	}

	const u64 OpenGL_Buffer::GetGPUAdress()
	{
		return 0;
	}
}