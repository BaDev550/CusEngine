#pragma once

#include <Runtime/RHI/Object/RHIObject.h>
#include <Runtime/RHI/Image/RHIImageDesc.h>

namespace Runtime::RHI {
	class Image : public Object {
	public:
		using Object::Object;
		virtual ~Image() = default;

		virtual void SetDesc(const ImageDesc& desc) = 0;
		virtual ImageDesc* GetDesc() = 0;
		virtual const Format GetFormat() const = 0;
		virtual const u32 GetWidth() const noexcept = 0;
		virtual const u32 GetHeight() const noexcept = 0;
		virtual u32 GetBindlessIndex() noexcept = 0;
		virtual u32 GetSamplerIndex() noexcept = 0;
	};
}