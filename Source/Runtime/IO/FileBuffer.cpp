#include "FileBuffer.h"
#include <chrono>
#include <fstream>
#include <sys/stat.h>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Profiler.h>

#include <mio/mmap.hpp>

namespace CusEngine {
	FileBuffer::FileBuffer(std::string_view path) : _path(path.data()) {
		//_buff = fopen();
	}

	void FileBuffer::Stream(std::vector<u8>& result, std::string_view path) {
		Logger::Info("FileBuffer", "Streaming file: {}", path.data());
		
		BEGIN_SCOPE(FileBufferMIOReadTime);
		std::error_code err;
		mio::mmap_source mmap(path.data());
		if (err) return;

		result.assign(mmap.begin(), mmap.end());
		END_SCOPE(FileBufferMIOReadTime);
	}

	std::vector<u8> FileBuffer::Read(std::string_view path) {
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

			std::vector<u8> fileBuffer((buffSize + sizeof(u8) - 1) / sizeof(u8));
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

	bool FileBuffer::Write(std::string_view path, void* data, usize size, FileWritingMethod method, usize offset) {
		const char* op = (method == FileWritingMethod::Binary) ? (offset > 0) ? "r+b" : "wb" : (offset > 0) ? "r" : "w";
		FILE* buff = fopen(path.data(), op);
		if (buff) {
			BEGIN_SCOPE(FileBufferWrite);
			fseek(buff, static_cast<long>(offset), SEEK_SET);
			fwrite(data, size, 1, buff);
			fclose(buff);
			END_SCOPE(FileBufferWrite);
			return true;
		}
		else {
			Logger::Error("FileBuffer", "Failed to open buffer");
			return false;
		}
	}
}