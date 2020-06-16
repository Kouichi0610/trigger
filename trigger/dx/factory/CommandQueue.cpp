#include "CommandQueue.h"
#include "Device.h"

namespace dx::factory {
	ComPtr<ID3D12CommandQueue> CommandQueue::Get() const {
		return queue;
	}
	CommandQueue::CommandQueue(const Device& device, std::shared_ptr<logger::ILogger> logger) {
		auto desc = D3D12_COMMAND_QUEUE_DESC{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		auto result = device.Get()->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
		logger->CheckError(result, "CreateCommandQueue");
	}
}
