#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "IModelData.h"

namespace dx {
	using Microsoft::WRL::ComPtr;
	class VertexShader;
	class PixelShader;
	class ITexture;

	class Pmd final : public IModelData {
	public:
		Pmd(ComPtr<ID3D12Device>, const char*, ITexture*, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>, const XMMATRIX&);

		Pmd(const Pmd&) = delete;
		virtual ~Pmd() = default;
	private:
		const size_t VertexSize = 38;

		ComPtr<ID3D12Resource> vertexBuffer;
		ComPtr<ID3D12Resource> indexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexView;
		D3D12_INDEX_BUFFER_VIEW indexView;
		UINT vertexCount;
		UINT indexCount;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;

		ComPtr<ID3D12DescriptorHeap> heapSrv;

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12Resource> constBuff;

		// TODO:RootDescriptorŠÖ˜A
		UINT descriptorHandleIncrementSize;

		XMMATRIX* map;

		void createVertices(ComPtr<ID3D12Device>, std::vector<unsigned char>);
		void createIndices(ComPtr<ID3D12Device>, std::vector<unsigned short>);
		void createRootSignature(ComPtr<ID3D12Device> device);
		void createPipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);
	};
}
