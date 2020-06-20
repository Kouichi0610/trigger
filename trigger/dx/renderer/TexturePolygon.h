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

#include "components/VertexBuffer.h"
#include "components/IndexBuffer.h"

using namespace DirectX;

namespace dx {
	using Microsoft::WRL::ComPtr;
	class VertexBuffer;
	class IndexBuffer;
	class TextureBuffer;
	class PixelShader;
	class VertexShader;

	class TexturePolygon : public IModel {
	public:
		TexturePolygon(ComPtr<ID3D12Device>, std::vector<TextureVertex>, std::vector<unsigned short>, std::vector<TexRGBA> texture, int w, int h,  std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);

		virtual ~TexturePolygon() = default;
		TexturePolygon(const TexturePolygon&) = delete;
	private:
		D3D12_INDEX_BUFFER_VIEW ibView;
		D3D12_VERTEX_BUFFER_VIEW vbView;

		std::unique_ptr<VertexBuffer> vertexBuffer;
		std::unique_ptr<IndexBuffer> indexBuffer;
		std::unique_ptr<TextureBuffer> textureBuffer;

		ComPtr<ID3D12DescriptorHeap> heapSrv;

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;

		int indicesCount;

		ComPtr<ID3D12RootSignature> createRootSignature(ComPtr<ID3D12Device>) const;
		ComPtr<ID3D12PipelineState> createPipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature) const;
	};
}
