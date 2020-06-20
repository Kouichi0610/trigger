#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include <string>
#include "../../logger/ILogger.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	class VertexShader final {
	public:
		ComPtr<ID3DBlob> Get() const;
		VertexShader(LPCWSTR, std::string, std::shared_ptr<logger::ILogger>);

		virtual ~VertexShader() = default;
		VertexShader(const VertexShader&) = delete;
	private:
		ComPtr<ID3DBlob> shader;
	};

}
