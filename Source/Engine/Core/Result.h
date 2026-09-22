#pragma once
#include <Engine/Core/Types.h>
#include <string>

namespace CusEngine {
	class Result final {
	public:
		Result(std::string_view msg = "") : _msg(msg) {}
		~Result() = default;

		std::string GetMessage() const { return _msg; }

		operator bool() const { return _msg.empty(); }
	private:
		std::string _msg;
	};
}