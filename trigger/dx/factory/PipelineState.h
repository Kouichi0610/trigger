#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	class PipelineState final {
	public:
		ComPtr<ID3D12PipelineState> Get() const;
		PipelineState(
			ComPtr<ID3D12Device> device,
			ComPtr<ID3D12RootSignature> rootSignature,
			ComPtr<ID3DBlob> vertexShader,
			ComPtr<ID3DBlob> pixelShader,
			std::shared_ptr<logger::ILogger> logger);

		virtual ~PipelineState() = default;
		PipelineState(const PipelineState&) = delete;
	private:
		ComPtr<ID3D12PipelineState> pipelineState;
	};
}

