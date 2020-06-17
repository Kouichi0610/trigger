#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "DXBase.h"
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

#include "renderer/ModelInfo.h"
#include "renderer/SamplePolygon.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

namespace dx {
	void DXBase::Main() {
		auto idx = swapChain->GetCurrentBackBufferIndex();
		auto backBuffer = renderTargets[idx];

		// ���\�[�X�o���A�̐ݒ�
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
		// �`��Ώۂ��o�b�N�o�b�t�@�ɐݒ�
		auto handle = heapRtv->GetCPUDescriptorHandleForHeapStart();
		auto size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		handle.ptr += idx * size;
		commandList->OMSetRenderTargets(1, &handle, true, nullptr);

		// ��ʂ��N���A
		{
			float color[] = { 0.3f, 0.0f, 0.2f, 1.0f };
			commandList->ClearRenderTargetView(handle, color, 0, nullptr);
		}
		// �f�v�X�o�b�t�@�̃N���A
		{
			auto handle = heapDsv->GetCPUDescriptorHandleForHeapStart();
			commandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, 1.0F, 0, 0, nullptr);

		}

		// TODO:�p�C�v���C���X�e�[�g�̐ݒ肪�悩������Ȃ�
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorrect);

		for (const auto model : models) {
			model->Render(commandList);
		}

		// ���\�[�X�o���A�����Ƃɖ߂�
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
		// ���ߎ�t�I��
		commandList->Close();

		// �R�}���h���X�g�����s����
		{
			ID3D12CommandList* commands[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists(1, commands);
		}
		// ���s������҂�
		{
			fenceValue++;
			commandQueue->Signal(fence.Get(), fenceValue);

			if (fence->GetCompletedValue() != fenceValue) {
				auto event = CreateEvent(nullptr, false, false, nullptr);
				fence->SetEventOnCompletion(fenceValue, event);

				WaitForSingleObject(event, INFINITE);
				CloseHandle(event);
			}
		}

		// ���s�����B�R�}���h���X�g���N���A
		commandAllocators[idx]->Reset();
		// �N���[�Y������
		commandList->Reset(commandAllocators[idx].Get(), nullptr);

		// �t���b�v
		swapChain->Present(1, 0);
	}

	void DXBase::Initialize(HWND hwnd) {
		auto factory = factory::Factory(logger);
		auto device = factory::Device(logger);
		auto commandQueue = factory::CommandQueue(device, logger);
		auto swapchain = factory::SwapChain(hwnd, factory, commandQueue, FrameBufferCount, logger);
		auto commandAllocator = factory::CommandAllocator(device, FrameBufferCount, logger);
		auto commandList = factory::CommandList(device, commandAllocator, logger);
		auto heapRtv = factory::HeapRtv(device, FrameBufferCount, logger);
		auto renderTargets = factory::RenderTargets(device, swapchain, heapRtv, FrameBufferCount, logger);
		auto heapDsv = factory::HeapDsv(device, logger);

		auto width = swapchain.Width();
		auto height = swapchain.Height();

		auto depthBuffer = factory::DepthBuffer(device, heapDsv, width, height, logger);
		auto fence = factory::Fence(device, logger);

		this->factory = factory.Get();
		this->device = device.Get();
		this->commandQueue = commandQueue.Get();
		this->swapChain = swapchain.Get();
		this->commandAllocators = commandAllocator.Get();
		this->commandList = commandList.Get();
		this->heapRtv = heapRtv.Get();
		this->renderTargets = renderTargets.Get();
		this->heapDsv = heapDsv.Get();
		this->depthBuffer = depthBuffer.Get();
		this->fence = fence.Get();
		this->fenceValue = fence.FenceValue();

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
		fence = nullptr;
		depthBuffer = nullptr;
		heapDsv = nullptr;
		renderTargets.clear();
		heapRtv = nullptr;
		commandList = nullptr;
		commandAllocators.clear();
		swapChain = nullptr;
		commandQueue = nullptr;
		device = nullptr;
		factory = nullptr;
		logger = nullptr;
	}

	DXBase::DXBase(std::shared_ptr<logger::ILogger> logger) {
		this->logger = logger;
	}
	DXBase::~DXBase() {
		Terminate();
	}

	std::shared_ptr<ModelInfo> DXBase::CreateModelInfo() const {
		return std::make_shared<ModelInfo>(device, logger);
	}
	void DXBase::Entry(std::shared_ptr<ModelBase> model) {
		models.emplace_back(model);
	}
}
