#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	/// <summary>
	/// ディスクリプタヒープ
	/// ConstantBufferView, ShaderResourceView, UnorderedAccessView
	/// 
	/// </summary>
	class HeapSrv final {
	public:
		ComPtr<ID3D12DescriptorHeap> Get() const;
		HeapSrv(ComPtr<ID3D12Device>, int);

		virtual ~HeapSrv() = default;
		HeapSrv(const HeapSrv&) = delete;
	private:
		ComPtr<ID3D12DescriptorHeap> heapSrv;
	};
}
