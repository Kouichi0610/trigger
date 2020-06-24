#include "CommandExecutor.h"

namespace dx {

	void CommandExecutor::Execute(ComPtr<ID3D12CommandList> list) {
		ID3D12CommandList* commands[] = { list.Get() };
		Execute(1, commands);
	}
	void CommandExecutor::Execute(UINT commandCount, ID3D12CommandList* commands[]) {
		// ŽÀs
		commandQueue->ExecuteCommandLists(commandCount, commands);

		// ŽÀsŠ®—¹‚Ü‚Å‘Ò‚Â
		fenceValue++;
		commandQueue->Signal(fence.Get(), fenceValue);

		if (fence->GetCompletedValue() != fenceValue) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceValue, event);

			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
	CommandExecutor::CommandExecutor(ComPtr<ID3D12CommandQueue> queue, ComPtr<ID3D12Fence> fence) {
		this->commandQueue = queue;
		this->fence = fence;
		fenceValue = 0;
	}
}
