#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	/// <summary>
	/// Depth Stencil View
	/// </summary>
	class HeapDsv final {
	public:
		ComPtr<ID3D12DescriptorHeap> Get() const;
		HeapDsv(const Device&);

		virtual ~HeapDsv() = default;
		HeapDsv(const HeapDsv&) = delete;
	private:
		ComPtr<ID3D12DescriptorHeap> heapDsv;
	};
}

