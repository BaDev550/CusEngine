#pragma once

#include <Engine/Core/Core.h>

#include <Runtime/RHI/Object/RHIObject.h>
#include <Runtime/RHI/Buffer/RHIBufferDesc.h>

namespace CusEngine::RHI {
	class ENGINE_API Buffer : public RHIObject {
	public:
		virtual ~Buffer() = default;

		virtual void Write(const void* data, size_t size = SIZE_MAX, size_t offset = SIZE_MAX) = 0;
		virtual const void* GetMappedPtr() const = 0;

		virtual std::string_view GetObjectDebugName() const override = 0;
		virtual const size_t GetSize() const noexcept = 0;
		virtual const BufferDesc* GetDesc() const = 0;
		virtual const BufferUsage GetUsage() const = 0;
		virtual const MemoryUsage GetMemoryUsage() const = 0;
		virtual const u64 GetGPUAdress() = 0;
	};
}