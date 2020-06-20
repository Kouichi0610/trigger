#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class RootSignature final {
	public:
		ComPtr<ID3D12RootSignature> Get() const;
		RootSignature(ComPtr<ID3D12Device>);
		// TODO:with texture

		virtual ~RootSignature() = default;
		RootSignature(const RootSignature&) = delete;
	private:
		ComPtr<ID3D12RootSignature> rootSignature;
	};

}
