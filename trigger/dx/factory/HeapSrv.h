#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	/// <summary>
	/// Shader Resource View
	/// 用のディスクリプタヒープ
	/// </summary>
	class HeapSrv final {
	public:
		ComPtr<ID3D12DescriptorHeap> Get() const;
		HeapSrv(ComPtr<ID3D12Device>, std::shared_ptr<logger::ILogger>);

		virtual ~HeapSrv() = default;
		HeapSrv(const HeapSrv&) = delete;
	private:
		ComPtr<ID3D12DescriptorHeap> heapSrv;
	};
}
