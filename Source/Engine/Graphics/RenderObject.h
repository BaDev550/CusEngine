#pragma once

#include "RHI/RHI.h"
#include "Core/Ref.h"

namespace CusEngine {
	class RenderObject : public Mem::RefCounted {
	public:
		friend class RHI::RenderCommands;
		friend class RHI::RenderContext;

		RenderObject(RHI::RenderCommands* commands) : _commands(commands) {}
		virtual ~RenderObject() = default;

		[[nodiscard]] virtual std::string_view GetObjectDebugName() const = 0;
		
		template<class T = RHI::RenderCommands>
		T* GetRenderCommands() { return static_cast<T*>(_commands); }
	private:
		RHI::RenderCommands* _commands = nullptr;
	};
}