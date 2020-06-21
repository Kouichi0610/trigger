#include "HeapRtv.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<ID3D12DescriptorHeap> HeapRtv::Get() const {
		return heapRtv;
	}
	HeapRtv::HeapRtv(const Device& device, UINT frameBufferCount) {
		auto desc = D3D12_DESCRIPTOR_HEAP_DESC {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NodeMask = 0;	// GPU‚ª•¡”‚ ‚éê‡AŽ¯•Ê‚·‚é‚½‚ß‚Ì
		desc.NumDescriptors = 2;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		auto result = device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapRtv));
		logger::CheckError(result, "CreateRTV");
	}
}