#pragma once

#include <Runtime/RHI/Object/RHIObject.h>
#include <Runtime/RHI/Image/RHIImageDesc.h>

namespace CusEngine::RHI {
	class ENGINE_API Image : public RHIObject {
	public:
		virtual ~Image() = default;

		virtual std::string_view GetObjectDebugName() const override = 0;
		virtual const ImageDesc* GetDesc() const = 0;
		virtual const Format GetFormat() const = 0;
		virtual const u32 GetWidth() const noexcept = 0;
		virtual const u32 GetHeight() const noexcept = 0;
	};
}