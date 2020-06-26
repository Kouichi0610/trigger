#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "DXBase.h"
#include "CommandExecutor.h"

#include "factory/Factory.h"
#include "factory/Device.h"
#include "factory/CommandQueue.h"
#include "factory/SwapChain.h"
#include "factory/CommandAllocator.h"
#include "factory/CommandList.h"
#include "factory/HeapRtv.h"
#include "factory/HeapDsv.h"
#include "factory/DepthBuffer.h"
#include "factory/RenderTargets.h"
#include "factory/Fence.h"

#include "ModelFactory.h"
#include "renderer/IModel.h"
#include "renderer/texture/Texture.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

namespace dx {
	void DXBase::Main() {
		auto idx = swapChain->GetCurrentBackBufferIndex();
		auto backBuffer = renderTargets[idx];

		// リソースバリアの設定
		{
			auto desc = D3D12_RESOURCE_BARRIER{};
			desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			desc.Transition.pResource = backBuffer.Get();
			desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			desc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			desc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			commandList->ResourceBarrier(1, &desc);
		}
		// 描画対象をバックバッファに設定
		auto handle = heapRtv->GetCPUDescriptorHandleForHeapStart();
		auto size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		handle.ptr += idx * size;
		commandList->OMSetRenderTargets(1, &handle, true, nullptr);

		// 画面をクリア
		{
			float color[] = { 0.3f, 0.0f, 0.2f, 1.0f };
			commandList->ClearRenderTargetView(handle, color, 0, nullptr);
		}
		// デプスバッファのクリア
		{
			auto handle = heapDsv->GetCPUDescriptorHandleForHeapStart();
			commandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, 1.0F, 0, 0, nullptr);

		}

		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorrect);

		for (const auto model : models) {
			model->Render(commandList);
		}

		// リソースバリアをもとに戻す
		{
			D3D12_RESOURCE_BARRIER desc = {};
			desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			desc.Transition.pResource = backBuffer.Get();
			desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			desc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			commandList->ResourceBarrier(1, &desc);
		}
		// 命令受付終了
		commandList->Close();


		// CommandQueue コマンドリストの実行＆実行完了
		commandExecutor->Execute(commandList);

		// 実行完了。コマンドリストをクリア
		commandAllocators[idx]->Reset();
		// クローズを解除
		commandList->Reset(commandAllocators[idx].Get(), nullptr);

		// フリップ
		swapChain->Present(1, 0);
	}

	void DXBase::Initialize(HWND hwnd) {
		// DirectXTex関連のために必要
		auto result = CoInitializeEx(0, COINIT_MULTITHREADED);

		auto factory = factory::Factory();
		auto device = factory::Device();
		auto commandQueue = factory::CommandQueue(device);
		auto swapchain = factory::SwapChain(hwnd, factory, commandQueue, FrameBufferCount);
		auto commandAllocator = factory::CommandAllocator(device, FrameBufferCount);
		auto commandList = factory::CommandList(device, commandAllocator);
		auto heapRtv = factory::HeapRtv(device, FrameBufferCount);
		auto renderTargets = factory::RenderTargets(device, swapchain, heapRtv, FrameBufferCount);
		auto heapDsv = factory::HeapDsv(device);

		auto width = swapchain.Width();
		auto height = swapchain.Height();

		auto depthBuffer = factory::DepthBuffer(device, heapDsv, width, height);
		auto fence = factory::Fence(device);

		this->factory = factory.Get();
		this->device = device.Get();
		this->swapChain = swapchain.Get();
		this->commandAllocators = commandAllocator.Get();
		this->commandList = commandList.Get();
		this->heapRtv = heapRtv.Get();
		this->renderTargets = renderTargets.Get();
		this->heapDsv = heapDsv.Get();
		this->depthBuffer = depthBuffer.Get();

		commandExecutor = std::make_unique<CommandExecutor>(commandQueue.Get(), fence.Get());

		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		scissorrect.top = 0;
		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + width;
		scissorrect.bottom = scissorrect.top + height;
	}
	void DXBase::Terminate() {
		depthBuffer = nullptr;
		heapDsv = nullptr;
		renderTargets.clear();
		heapRtv = nullptr;
		commandList = nullptr;
		commandAllocators.clear();
		swapChain = nullptr;
		device = nullptr;
		factory = nullptr;
	}

	DXBase::DXBase() {
	}
	DXBase::~DXBase() {
		Terminate();
	}

	void DXBase::Entry(std::shared_ptr<IModel> model) {
		models.emplace_back(model);
	}

	std::unique_ptr<ModelFactory> DXBase::CreateModelFactory() const {
		return std::make_unique<ModelFactory>(device);
	}

	std::unique_ptr<Texture> DXBase::CreateTexture() const {
		return std::make_unique<Texture>(device, commandList, commandExecutor.get());
	}
}
