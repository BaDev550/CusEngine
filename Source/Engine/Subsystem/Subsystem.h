#pragma once

#include "Core/Types.h"
#include <string>
#include <typeinfo>

namespace CusEngine {
	enum class SubsystemOrder {
		Before = 0,
		After
	};

	class Engine;
	class Subsystem {
	public:
		virtual ~Subsystem() = default;

		virtual bool OnCreate(Engine* engine) { _engine = engine; return true; }
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}

		virtual SubsystemOrder GetInitOrder() { return SubsystemOrder::After; }
		virtual SubsystemOrder GetCreateOrder() { return SubsystemOrder::After; }

		const std::type_info& GetTypeID() { return typeid(*this); }
	private:
		Engine* _engine;
	};
}