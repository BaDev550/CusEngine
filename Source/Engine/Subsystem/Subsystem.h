#pragma once

#include "Core/Core.h"
#include "Core/Types.h"
#include "Core/Object.h"
#include <string>
#include <typeinfo>
#include <typeindex>
#include <list>
#include <set>

namespace CusEngine {
	enum class DependencyOrder {
		Before = 0,
		After
	};

	constexpr inline std::string_view OrderToString(DependencyOrder order) {
		switch (order)
		{
		case CusEngine::DependencyOrder::Before: return "Before";
		case CusEngine::DependencyOrder::After: return "After";
		default:
			break;
		}
	}

	class DependencyGraph final {
	public:
		using DependencyList = std::list<std::pair<std::type_index, DependencyOrder>>;

		template<typename T>
		void Require(DependencyOrder order) {
			_list.emplace_back(typeid(T), order);
			//Logger::Info(typeid(*this).name(), "requires {} to initialized {}", typeid(T).name(), OrderToString(order));
		}

		[[nodiscard]] const DependencyList& GetList() const { return _list; }
	private:
		DependencyList _list;
	};

	class Engine;

	CUS_CLASS()
	class ENGINE_API Subsystem : public Object {
		REFLECT_CLASS(Subsystem)
	public:
		virtual ~Subsystem() = default;

		virtual bool OnInitialized(Engine* engine) { return true; }
		virtual bool OnCreate(Engine* engine) { 
			_engine = engine; 
			Logger::Info(typeid(*this).name(), "Created!");
			return true;
		}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}

		virtual void GetDependencyGraph(DependencyGraph& graph) { }
		
		const std::type_info& GetTypeID() const { return typeid(*this); }
	protected:
		CUS_PROP();
		Engine* _engine;
	};
}