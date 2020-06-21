#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
																				
namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;
	class Factory;
	class CommandQueue;

	// TODO:RenderTarget(BufferCountï™)çÏê¨
	class SwapChain final {
	public:
		int Width() const;
		int Height() const;
		ComPtr<IDXGISwapChain4> Get() const;
		SwapChain(HWND, const Factory&, const CommandQueue&, UINT);

		virtual ~SwapChain() = default;
		SwapChain(const SwapChain&) = delete;
	private:
		ComPtr<IDXGISwapChain4> swapchain;
		int width;
		int height;
	};
}
