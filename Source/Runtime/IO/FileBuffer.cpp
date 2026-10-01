#include "FileBuffer.h"
#include <sys/stat.h>

#include <Runtime/Definitions/Logger.h>
#include <Runtime/Definitions/Profiler.h>

#include <mio/mmap.hpp>

namespace Runtime::IO {
	Result FileBuffer::Map(std::string_view path, std::vector<u8>& result) {
		Logger::Info("FileBuffer", "Streaming file: {}", path.data());
		
		BEGIN_SCOPE(FileBufferMemoryBufferReadTime);
		std::error_code err;
		mio::mmap_source mmap(path.data());
		if (err) return Result(err.message());

		result.assign(mmap.begin(), mmap.end());
		END_SCOPE(FileBufferMemoryBufferReadTime);
	}

	std::vector<u8> FileBuffer::ReadBinary(std::string_view path) {
		std::string pathStr(path);

		struct stat st;
		
		if (stat(pathStr.c_str(), &st) == 0) {
			usize buffSize = static_cast<usize>(st.st_size);
			float buffSizeMB = ((float)buffSize / 1024 / 1024);

			if (!buffSize) {
				Logger::Warn("FileBuffer", "Empty file");
				return {};
			}

			Logger::Info("FileBuffer", "Reading buffer: {}", pathStr);
			Logger::Info("FileBuffer", "size: {:.2f}MB", buffSizeMB);

			std::vector<u8> fileBuffer((buffSize + sizeof(u8) - 1) / sizeof(u8));
			BEGIN_SCOPE(FileBufferBinaryReadTime);
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
			return fileBuffer;
		}
		else {
			Logger::Warn("FileBuffer", "Failed to read buffer size");
			return {};
		}
		return {};
	}

	Result FileBuffer::WriteBinary(std::string_view path, void* data, usize size, usize offset) {
		const char* op = (offset > 0) ? "r+b" : "wb";
		FILE* buff = fopen(path.data(), op);
		if (buff) {
			BEGIN_SCOPE(FileBufferWrite);
			fseek(buff, static_cast<long>(offset), SEEK_SET);
			fwrite(data, size, 1, buff);
			fclose(buff);
			END_SCOPE(FileBufferWrite);
			return Result();
		}
		else {
			return Result("Failed to open buffer");
		}
	}
}