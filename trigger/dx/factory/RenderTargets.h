#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;
	class SwapChain;
	class HeapRtv;

	/// <summary>
	/// バックバッファを生成、取得
	/// </summary>
	class RenderTargets final {
	public:
		std::vector<ComPtr<ID3D12Resource>> Get() const;
		RenderTargets(const Device&, const SwapChain&, const HeapRtv&, UINT);

		virtual ~RenderTargets() = default;
		RenderTargets(const RenderTargets&) = delete;
	private:
		std::vector<ComPtr<ID3D12Resource>> renderTargets;	// BackBuffer
	};

}
