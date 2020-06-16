#include "Logger.h"
#include <iostream>
#include <stdarg.h>
#include <string>

namespace logger {
	void Logger::Log(const char* format, ...) {
		va_list valist;
		va_start(valist, format);
		vprintf(format, valist);
		va_end(valist);
	}
	void Logger::CheckError(HRESULT result, const char* msg) {
		if (result == S_OK) return;
		Log("failed %s Result[%x]\n", msg, result);
		exit(1);
	}
	void Logger::CheckError(HRESULT result, const ComPtr<ID3DBlob> error, const char* msg) {
		if (result == S_OK) return;
		if (error != nullptr) {
			std::string errorMsg;
			errorMsg.resize(error->GetBufferSize());
			std::copy_n((char*)error->GetBufferPointer(), error->GetBufferSize(), errorMsg.begin());
			Log("failed %s Result[%x] [%s]\n", msg, result, errorMsg.c_str());
		}
		else {
			Log("failed %s Result[%x]\n", msg, result);
		}
		exit(1);
	}
}
