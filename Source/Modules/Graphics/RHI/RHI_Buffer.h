#pragma once

#include "RHI.h"
#include "RHI_Object.h"

namespace Graphics {
	class ENGINE_API RHI_Buffer : public RHI_Object {
	public:
		RHI_Buffer(RHI_RenderCommands* commands) : RHI_Object(commands) {}
		RHI_Buffer() : RHI_Object(nullptr) {}
		virtual ~RHI_Buffer() = default;

		virtual void Write(const void* data, size_t size = SIZE_MAX, size_t offset = SIZE_MAX) = 0;
		virtual const void* GetMappedPtr() const = 0;

		virtual std::string_view GetObjectDebugName() const override = 0;
		virtual RHI_BufferHandle GetNativeHandle() const = 0;
		virtual const size_t GetSize() const noexcept = 0;
		virtual const RHI_BufferDesc* GetDesc() const = 0;
		virtual const RHI_BufferUsage GetUsage() const = 0;
		virtual const RHI_MemoryUsage GetMemoryUsage() const = 0;
		virtual const u64 GetGPUAdress() = 0;
	};
}