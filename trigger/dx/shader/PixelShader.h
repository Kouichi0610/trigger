#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include <string>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class PixelShader final {
	public:
		ComPtr<ID3DBlob> Get() const;
		PixelShader(LPCWSTR, std::string);

		virtual ~PixelShader() = default;
		PixelShader(const PixelShader&) = delete;
	private:
		ComPtr<ID3DBlob> shader;
	};

}
