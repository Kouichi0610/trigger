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
	class PixelShader;
	class VertexShader;

	class Polygon : public IModel {
	public:

		Polygon(ComPtr<ID3D12Device>, std::vector<Vertex>, std::vector<unsigned short>, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);

		virtual ~Polygon() = default;
		Polygon(const Polygon&) = delete;
	private:
		D3D12_INDEX_BUFFER_VIEW ibView;
		D3D12_VERTEX_BUFFER_VIEW vbView;

		std::unique_ptr<VertexBuffer> vertexBuffer;
		std::unique_ptr<IndexBuffer> indexBuffer;

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;

		int indicesCount;
	};
}
