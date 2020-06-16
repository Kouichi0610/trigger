#include "Fence.h"
#include "Device.h"

namespace dx::factory {
	ComPtr<ID3D12Fence> Fence::Get() const {
		return fence;
	}
	int Fence::FenceValue() const {
		return fenceValue;
	}
	Fence::Fence(const Device& device, std::shared_ptr<logger::ILogger> logger) {
		auto result = device.Get()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		logger->CheckError(result, "CreateFence");
	}
}
