#include "SwapChain.h"
#include "Device.h"
#include "Factory.h"
#include "CommandQueue.h"

namespace dx::factory {
	int SwapChain::Width() const {
		return width;
	}
	int SwapChain::Height() const {
		return height;
	}
	ComPtr<IDXGISwapChain4> SwapChain::Get() const {
		return swapchain;
	}
	SwapChain::SwapChain(HWND hwnd, const Factory& factory, const CommandQueue& commandQueue, UINT frameBufferCount, std::shared_ptr<logger::ILogger> logger) {
		// �E�B���h�E�n���h������T�C�Y�擾s
		RECT r;
		GetClientRect(hwnd, &r);
		auto width = r.right - r.left;
		auto height = r.bottom - r.top;
		this->width = width;
		this->height = height;

		auto desc = DXGI_SWAP_CHAIN_DESC1{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;	// 3D�f�B�X�v���C�p
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		desc.BufferCount = frameBufferCount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// �t���b�v��A�o�b�t�@���e�𑬂₩�ɔj��
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// �w��Ȃ�
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E<->�t���X�N���[���؂�ւ��\

		ComPtr<IDXGISwapChain1> sc;
		auto result = factory.Get()->CreateSwapChainForHwnd(
			commandQueue.Get().Get(),
			hwnd,
			&desc,
			nullptr,
			nullptr,
			&sc
		);
		logger->CheckError(result, "CreateSwapChan");
		// SwapChain4�ɕϊ�
		sc.As(&swapchain);
	}
}
