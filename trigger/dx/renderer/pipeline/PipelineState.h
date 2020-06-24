#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class PipelineState final {
	public:
		ComPtr<ID3D12PipelineState> Get() const;
		PipelineState(ComPtr<ID3D12Device>, ComPtr<ID3D12RootSignature>, ComPtr<ID3DBlob> vs, ComPtr<ID3DBlob> ps);
		// TODO:with texture

		virtual ~PipelineState() = default;
		PipelineState(const PipelineState&) = delete;
	private:
		ComPtr<ID3D12PipelineState> pipelineState;
	};

}
