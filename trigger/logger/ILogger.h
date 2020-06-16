#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace logger {
	using Microsoft::WRL::ComPtr;

	class ILogger {
	public:
		virtual void Log(const char* format, ...) = 0;
		virtual void CheckError(HRESULT result, const char* msg) = 0;
		virtual void CheckError(HRESULT result, const ComPtr<ID3DBlob> error, const char* msg) = 0;

		virtual ~ILogger() {}
	};
}
