#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;
	class Factory;
	class CommandQueue;

	// TODO:RenderTarget(BufferCount��)�쐬
	class SwapChain final {
	public:
		int Width() const;
		int Height() const;
		ComPtr<IDXGISwapChain4> Get() const;
		SwapChain(HWND, const Factory&, const CommandQueue&, UINT, std::shared_ptr<logger::ILogger>);

		virtual ~SwapChain() = default;
		SwapChain(const SwapChain&) = delete;
	private:
		ComPtr<IDXGISwapChain4> swapchain;
		int width;
		int height;
	};
}
