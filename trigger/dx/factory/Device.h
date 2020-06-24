#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	class Device final {
	public:
		ComPtr<ID3D12Device> Get() const;
		Device();

		virtual ~Device() = default;
		Device(const Device&) = delete;
	private:
		D3D_FEATURE_LEVEL GetNewestLevel() const;
		ComPtr<ID3D12Device> device;
	};
}
