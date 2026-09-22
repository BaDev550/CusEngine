#include "FileBuffer.h"
#include <chrono>
#include <fstream>
#include <sys/stat.h>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Profiler.h>

namespace CusEngine {
	FileBuffer::FileBuffer(std::string_view path) : _path(path.data()) {}

	std::vector<u64> FileBuffer::Read(std::string_view path) {
		if (_path == nullptr && !path.empty()) _path = path.data();
		else if (_path != nullptr) { _path = _path; }
		else { Logger::Warn("FileBuffer", "No valid path provided"); return {}; }

		std::string pathStr(_path);

		struct stat st;
		
		if (stat(pathStr.c_str(), &st) == 0) {
			usize buffSize = static_cast<usize>(st.st_size);
			float buffSizeMB = ((float)buffSize / 1024 / 1024);

			if (!buffSize) { Logger::Warn("FileBuffer", "Empty file"); return {}; }
			Logger::Info("FileBuffer", "Reading buffer: {}", pathStr);
			Logger::Info("FileBuffer", "size: {:.2f}MB", buffSizeMB);

			std::vector<u64> fileBuffer((buffSize + sizeof(u64) - 1) / sizeof(u64));
			{
				BEGIN_SCOPE(FileBufferReadTime);
				FILE* file = fopen(pathStr.c_str(), "rb");
				if (file) {
					fread(&fileBuffer[0], 1, buffSize, file);
					fclose(file);
				}
				else {
					Logger::Error("FileBuffer", "Failed to read file {}", pathStr);
					return {};
				}
				END_SCOPE(FileBufferReadTime);
			}
			return fileBuffer;
		}
		else {
			Logger::Warn("FileBuffer", "Failed to read buffer size");
			return {};
		}
		return {};
	}
}