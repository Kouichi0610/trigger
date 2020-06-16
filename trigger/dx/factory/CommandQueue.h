#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	class CommandQueue final {
	public:
		ComPtr<ID3D12CommandQueue> Get() const;
		CommandQueue(const Device&, std::shared_ptr<logger::ILogger>);

		virtual ~CommandQueue() = default;
		CommandQueue(const CommandQueue&) = delete;
	private:
		ComPtr<ID3D12CommandQueue> queue;
	};
}
