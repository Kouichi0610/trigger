#include "HeapDsv.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<ID3D12DescriptorHeap> HeapDsv::Get() const {
		return heapDsv;
	}
	HeapDsv::HeapDsv(const Device& device) {
		auto desc = D3D12_DESCRIPTOR_HEAP_DESC {};
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		auto result = device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapDsv));
		logger::CheckError(result, "Create HeapDsv");
	}
}
