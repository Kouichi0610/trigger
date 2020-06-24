#include "CommandAllocator.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	std::vector<ComPtr<ID3D12CommandAllocator>> CommandAllocator::Get() const {
		return allocators;
	}

	CommandAllocator::CommandAllocator(const Device& device, UINT frameBufferCount) {
		allocators.resize(frameBufferCount);
		for (UINT i = 0; i < frameBufferCount; i++) {
			auto result = device.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocators[i]));
			logger::CheckError(result, "Create CommandAllocator");
		}
	}
}
