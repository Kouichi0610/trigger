#include "VertexShader.h"
#include <d3dcompiler.h>
#include "../../logger/Logger.h"

namespace dx {
	ComPtr<ID3DBlob> VertexShader::Get() const {
		return shader;
	}

	VertexShader::VertexShader(LPCWSTR file, std::string entry) {
		ComPtr<ID3DBlob> error;
		auto result = D3DCompileFromFile(
			file,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry.c_str(), "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&shader, &error);

		logger::CheckError(result, error, "Create VertexShader");
	}
}
