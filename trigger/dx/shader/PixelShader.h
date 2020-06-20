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

	class PixelShader final {
	public:
		ComPtr<ID3DBlob> Get() const;
		PixelShader(LPCWSTR, std::string, std::shared_ptr<logger::ILogger>);

		virtual ~PixelShader() = default;
		PixelShader(const PixelShader&) = delete;
	private:
		ComPtr<ID3DBlob> shader;
	};

}
