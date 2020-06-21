#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	class Fence final {
	public:
		ComPtr<ID3D12Fence> Get() const;
		int FenceValue() const;
		Fence(const Device&);

		virtual ~Fence() = default;
		Fence(const Fence&) = delete;
	private:
		ComPtr<ID3D12Fence> fence;
		const int fenceValue = 0;
	};
}
