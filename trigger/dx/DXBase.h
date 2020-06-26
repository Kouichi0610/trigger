#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class CommandExecutor;
	class ModelFactory;
	class IModel;
	class Texture;

	class DXBase {
	public:
		void Initialize(HWND hwnd);
		void Terminate();

		std::unique_ptr<Texture> CreateTexture() const;
		std::unique_ptr<ModelFactory> CreateModelFactory() const;
		void Entry(std::shared_ptr<IModel>);

		void Main();

		DXBase();
		virtual ~DXBase();

	private:
		ComPtr<IDXGIFactory6> factory;
		ComPtr<ID3D12Device> device;
		std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators;
		ComPtr<ID3D12GraphicsCommandList> commandList;

		ComPtr<IDXGISwapChain4> swapChain;

		ComPtr<ID3D12DescriptorHeap> heapRtv;
		std::vector<ComPtr<ID3D12Resource>> renderTargets;
		ComPtr<ID3D12DescriptorHeap> heapDsv;
		ComPtr<ID3D12Resource1> depthBuffer;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorrect;

		const UINT FrameBufferCount = 2;

		std::vector<std::shared_ptr<IModel>> models;

		std::unique_ptr<CommandExecutor> commandExecutor;
	};

}
