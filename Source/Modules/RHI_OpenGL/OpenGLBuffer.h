#pragma once

#include <Graphics/RHI/RHI_Buffer.h>

#include "OpenGLRenderContext.h"

namespace CusEngine::RHI {
	class OpenGL_Buffer final : public Buffer {
	public:
		OpenGL_Buffer(RenderCommands* commands, const BufferDesc& desc);
		virtual ~OpenGL_Buffer();

		virtual void Write(const void* data, usize size = SIZE_MAX, usize offset = SIZE_MAX) override final;
		virtual const void* GetMappedPtr() const override { return _mappedPtr; }

		virtual std::string_view GetObjectDebugName() const override { return "rhi_object_opengl_buffer"; };
		virtual const usize GetSize() const noexcept override final { return _desc.size; }
		virtual const BufferDesc* GetDesc() const override { return &_desc; }
		virtual const BufferUsage GetUsage() const override final { return _desc.usage; }
		virtual const MemoryUsage GetMemoryUsage() const override final { return _desc.memoryUsage; }
		virtual const u64 GetGPUAdress() override final;
	private:
		BufferDesc _desc;

		void* _mappedPtr = nullptr;
		size_t _allocationSize = SIZE_MAX;
		u64 _gpuAddress = 0;
	};
}