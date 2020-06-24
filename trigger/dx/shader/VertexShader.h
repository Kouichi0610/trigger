#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include <string>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class VertexShader final {
	public:
		ComPtr<ID3DBlob> Get() const;
		VertexShader(LPCWSTR, std::string);

		virtual ~VertexShader() = default;
		VertexShader(const VertexShader&) = delete;
	private:
		ComPtr<ID3DBlob> shader;
	};

}
