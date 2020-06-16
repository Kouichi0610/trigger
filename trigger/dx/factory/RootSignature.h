#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	class RootSignature final {
	public:
		ComPtr<ID3D12RootSignature> Get() const;
		RootSignature(ComPtr<ID3D12Device>, std::shared_ptr<logger::ILogger>);

		virtual ~RootSignature() = default;
		RootSignature(const RootSignature&) = delete;
	private:
		ComPtr<ID3D12RootSignature> rootSignature;
	};
}

