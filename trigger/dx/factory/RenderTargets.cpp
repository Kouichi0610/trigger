#include "RenderTargets.h"
#include "HeapRtv.h"
#include "Device.h"
#include "SwapChain.h"

namespace dx::factory {
	std::vector<ComPtr<ID3D12Resource>> RenderTargets::Get() const {
		return renderTargets;
	}
	/// <summary>
	/// バックバッファの取得
	/// ＆RenderTargetViewの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="swapchain"></param>
	/// <param name="heapRtv"></param>
	/// <param name="frameBufferCount"></param>
	/// <param name="logger"></param>
	RenderTargets::RenderTargets(
		const Device& device,
		const SwapChain& swapchain,
		const HeapRtv& heapRtv,
		UINT frameBufferCount,
		std::shared_ptr<logger::ILogger> logger) {

		renderTargets.resize(frameBufferCount);
		auto size = device.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (UINT i = 0; i < frameBufferCount; i++) {
			auto result = swapchain.Get()->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			logger->CheckError(result, "GetRenderTargets");

			// サイズ分ポインタをずらす
			auto handle = heapRtv.Get()->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += i * size;

			device.Get()->CreateRenderTargetView(
				renderTargets[i].Get(),
				nullptr,
				handle
			);
		}

	}
}
