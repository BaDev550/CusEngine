#pragma once
#include <Runtime/Definitions/Types.h>
#include <string>

namespace Runtime {
	class Result final {
	public:
		Result(std::string_view msg = "") : _msg(msg) {}
		~Result() = default;

		std::string GetMessage() const { return _msg.data(); }

		operator bool() const { return _msg.empty(); }
	private:
		std::string_view _msg;
	};
}