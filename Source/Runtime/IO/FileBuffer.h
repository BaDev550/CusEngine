#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Core.h>
#include <iostream>
#include <vector>
#include <string_view>

namespace CusEngine {
	enum class FileWritingMethod {
		Binary = 0,
		Text
	};

	class ENGINE_API FileBuffer final {
	public:
		FileBuffer(std::string_view path);
		FileBuffer() = default;

		std::vector<u8> Read(std::string_view path = "");
		bool Write(std::string_view path, void* data, usize size, FileWritingMethod method = FileWritingMethod::Binary, usize offset = 0);
		void ReadMIO(std::vector<u8>& result, std::string_view path);
	private:
		const char* _path = nullptr;
	};
}