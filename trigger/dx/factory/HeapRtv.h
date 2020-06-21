#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	class HeapRtv final {
	public:
		ComPtr<ID3D12DescriptorHeap> Get() const;
		HeapRtv(const Device&, UINT);

		virtual ~HeapRtv() = default;
		HeapRtv(const HeapRtv&) = delete;
	private:
		ComPtr<ID3D12DescriptorHeap> heapRtv;
	};
}
