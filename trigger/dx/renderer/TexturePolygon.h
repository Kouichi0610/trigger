#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "IModel.h"
#include "Vertex.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

using namespace DirectX;

namespace dx {
	using Microsoft::WRL::ComPtr;
	class ITexture;
	class VertexBuffer;
	class IndexBuffer;
	class PixelShader;
	class VertexShader;

	class TexturePolygon : public IModel {
	public:
		TexturePolygon(ComPtr<ID3D12Device>, std::vector<TextureVertex>, std::vector<unsigned short>, ITexture*,  std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);

		virtual ~TexturePolygon() = default;
		TexturePolygon(const TexturePolygon&) = delete;
	private:
		D3D12_INDEX_BUFFER_VIEW ibView;
		D3D12_VERTEX_BUFFER_VIEW vbView;

		std::unique_ptr<VertexBuffer> vertexBuffer;
		std::unique_ptr<IndexBuffer> indexBuffer;

		ComPtr<ID3D12DescriptorHeap> heapSrv;

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;

		int indicesCount;

		ComPtr<ID3D12RootSignature> createRootSignature(ComPtr<ID3D12Device>) const;
		ComPtr<ID3D12PipelineState> createPipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature) const;

		// TODO:íuÇ´èÍ
		ComPtr<ID3D12Resource> createMatrixConstBuffer(ComPtr<ID3D12Device>) const;
		ComPtr<ID3D12Resource> constBuff;

		// TODO:RootDescriptorä÷òA
		UINT descriptorHandleIncrementSize;

		XMMATRIX* map;
	};
}
