#pragma once

#include "RHI.h"
#include "RHI_Object.h"

namespace Graphics {
	class ENGINE_API RHI_Image : public RHI_Object {
	public:
		RHI_Image(RHI_RenderCommands* commands) : RHI_Object(commands) {}
		RHI_Image() : RHI_Object(nullptr) {}
		virtual ~RHI_Image() = default;

		virtual std::string_view GetObjectDebugName() const override = 0;
		virtual RHI_ImageHandle GetNativeHandle() const = 0;
		virtual const RHI_ImageDesc* GetDesc() const = 0;
		virtual const RHI_Format GetFormat() const = 0;
		virtual const u32 GetWidth() const noexcept = 0;
		virtual const u32 GetHeight() const noexcept = 0;
		virtual const u32 GetSamplerId() const noexcept = 0; // TEMP
		virtual void SetSamplerId(u32 id) = 0;
	};
}