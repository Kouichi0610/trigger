#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../logger/ILogger.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	class ModelBase;
	class ModelFactory;
	class IModel;

	class DXBase {
	public:
		void Initialize(HWND hwnd);
		void Terminate();

		std::unique_ptr<ModelFactory> CreateModelFactory() const;
		void Entry(std::shared_ptr<IModel>);

		void Main();

		DXBase(std::shared_ptr<logger::ILogger>);
		virtual ~DXBase();
	private:
		ComPtr<IDXGIFactory6> factory;
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12CommandQueue> commandQueue;
		std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators;
		ComPtr<ID3D12GraphicsCommandList> commandList;

		ComPtr<IDXGISwapChain4> swapChain;

		ComPtr<ID3D12DescriptorHeap> heapRtv;
		std::vector<ComPtr<ID3D12Resource>> renderTargets;
		ComPtr<ID3D12DescriptorHeap> heapDsv;
		ComPtr<ID3D12Resource1> depthBuffer;

		ComPtr<ID3D12Fence> fence;
		int fenceValue;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorrect;

		std::shared_ptr<logger::ILogger> logger;
		const UINT FrameBufferCount = 2;

		std::vector<std::shared_ptr<IModel>> models;
	};

}
