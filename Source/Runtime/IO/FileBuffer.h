#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/Result.h>
#include <iostream>
#include <vector>
#include <string_view>

namespace Runtime::IO {
	class FileBuffer final {
	public:
		static std::vector<u8> ReadBinary(std::string_view path);
		static Result WriteBinary(std::string_view path, void* data, usize size, usize offset = 0);
		static Result Map(std::string_view path, std::vector<u8>& result);
	};
}