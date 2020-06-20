#include "PixelShader.h"
#include <d3dcompiler.h>

namespace dx {
	ComPtr<ID3DBlob> PixelShader::Get() const {
		return shader;
	}

	PixelShader::PixelShader(LPCWSTR file, std::string entry, std::shared_ptr<logger::ILogger> logger) {
		ComPtr<ID3DBlob> error;
		auto result = D3DCompileFromFile(
			file,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry.c_str(), "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&shader, &error);

		logger->CheckError(result, error, "Create PixelShader");
	}
}