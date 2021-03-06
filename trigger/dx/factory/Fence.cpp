#include "Fence.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<ID3D12Fence> Fence::Get() const {
		return fence;
	}
	Fence::Fence(const Device& device) {
		const UINT fenceValue = 0;
		auto result = device.Get()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		logger::CheckError(result, "CreateFence");
	}
}
