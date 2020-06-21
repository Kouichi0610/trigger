#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;
	class CommandAllocator;

	class CommandList final {
	public:
		ComPtr<ID3D12GraphicsCommandList> Get() const;
		CommandList(const Device&, const CommandAllocator&);

		virtual ~CommandList() = default;
		CommandList(const CommandList&) = delete;
	private:
		ComPtr<ID3D12GraphicsCommandList> commandList;
	};

}
