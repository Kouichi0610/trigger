#include "CommandList.h"
#include "CommandAllocator.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<ID3D12GraphicsCommandList> CommandList::Get() const {
		return commandList;
	}
	CommandList::CommandList(const Device& device, const CommandAllocator& allocator) {
		auto result = device.Get()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			allocator.Get()[0].Get(),
			nullptr,
			IID_PPV_ARGS(&commandList)
		);
		logger::CheckError(result, "Create CommandList");
	}
}

