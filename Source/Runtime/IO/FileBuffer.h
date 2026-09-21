#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Core.h>
#include <iostream>
#include <vector>
#include <string_view>

namespace CusEngine {
	class ENGINE_API FileBuffer final {
	public:
		FileBuffer(std::string_view path);
		FileBuffer() = default;

		std::vector<u64> Read(std::string_view path = "");
	private:
		const char* _path = nullptr;
	};
}