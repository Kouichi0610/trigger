#pragma once
#include <wrl.h>
#include <iostream>
#include <stdarg.h>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace logger {
	using Microsoft::WRL::ComPtr;

	void Log(const char* format, ...);
	void CheckError(HRESULT result, const char* msg);
	void CheckError(HRESULT result, const ComPtr<ID3DBlob> error, const char* msg);
}
