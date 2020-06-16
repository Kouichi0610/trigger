#pragma once
#include "../ILogger.h"

namespace logger {
	class Logger final : public ILogger {
	public:
		void Log(const char* format, ...);
		void CheckError(HRESULT result, const char* msg);
		void CheckError(HRESULT result, const ComPtr<ID3DBlob> error, const char* msg);

		Logger() = default;
		virtual ~Logger() = default;
	};


}