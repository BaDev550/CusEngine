#pragma once

#include "Graphics/RHI/RHI.h"
#include "Graphics/RenderObject.h"

namespace CusEngine::RHI {
	class ENGINE_API Image : public RenderObject {
	public:
		Image(RenderCommands* commands) : RenderObject(commands) {}
		Image() : RenderObject(nullptr) {}
		virtual ~Image() = default;

		virtual std::string_view GetObjectDebugName() const override = 0;
		virtual const ImageDesc* GetDesc() const = 0;
		virtual const ImageFormat GetFormat() const = 0;
		virtual const u32 GetWidth() const noexcept = 0;
		virtual const u32 GetHeight() const noexcept = 0;
		//virtual const u32 GetSamplerId() const noexcept = 0; // TEMP
		//virtual void SetSamplerId(u32 id) = 0;
	};
}