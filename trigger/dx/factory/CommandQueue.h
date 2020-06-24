#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	class CommandQueue final {
	public:
		ComPtr<ID3D12CommandQueue> Get() const;
		CommandQueue(const Device&);

		virtual ~CommandQueue() = default;
		CommandQueue(const CommandQueue&) = delete;
	private:
		ComPtr<ID3D12CommandQueue> queue;
	};
}
