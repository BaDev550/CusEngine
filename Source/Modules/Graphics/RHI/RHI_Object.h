#pragma once

#include "RHI.h"

namespace Graphics {
	class ENGINE_API RHI_Object : public Memory::RefCounted {
	public:
		friend class RHI_RenderCommands;
		friend class RHI_RenderContext;

		RHI_Object(RHI_RenderCommands* commands) : _commands(commands) {}
		virtual ~RHI_Object() = default;

		[[nodiscard]] virtual std::string_view GetObjectDebugName() const = 0;
		
		template<typename T = RHI_RenderCommands>
		T* GetRenderCommands() { return static_cast<T*>(_commands); }
	private:
		RHI_RenderCommands* _commands = nullptr;
	};
}