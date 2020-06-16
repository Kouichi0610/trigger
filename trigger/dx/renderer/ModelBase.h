#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <string>
#include "../../logger/ILogger.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace dx {
	using Microsoft::WRL::ComPtr;

	enum class Topology : unsigned int {
		PointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		LineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		LineStrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	struct ModelInfo {
		LPCWSTR VertexShaderFile;
		std::string VertexShaderEntry;
		LPCWSTR PixelShaderFile;
		std::string PixelShaderEntry;
		UINT64 VerticesSize;
		UINT64 IndicesSize;
	};

	class ModelBase {
	public:
		virtual void Render(ComPtr<ID3D12GraphicsCommandList>) = 0;

		ModelBase();
		virtual void Init(ComPtr<ID3D12Device>, const ModelInfo&, std::shared_ptr<logger::ILogger>);
		virtual ~ModelBase() = default;
		ModelBase(const ModelBase&) = delete;
	protected:
		void render(ComPtr<ID3D12GraphicsCommandList>, Topology, int) const;
		virtual ComPtr<ID3D12PipelineState> createPipelineState(ComPtr<ID3D12Device>) = 0;

		ComPtr<ID3D12Resource> vertexBuffer;
		ComPtr<ID3D12Resource> indicesBuffer;
		ComPtr<ID3D12Resource> constantBuffer;

		// ファイル名、エントリ名
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12RootSignature> rootSignature;
		// 表示したいものに応じて
		ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_INDEX_BUFFER_VIEW ibView;
		D3D12_VERTEX_BUFFER_VIEW vbView;
		
		void mapVertex(std::vector<XMFLOAT3> vertices);
		//template <typename T>
		//void mapVertex(std::vector<T> vertices);
		void mapIndices(std::vector<unsigned short>);
		void mapIndices(std::vector<unsigned int>);

		std::shared_ptr<logger::ILogger> logger;
	};

#if false
	template <typename T>
	void ModelBase::mapVertex(std::vector<T> vertices) {
		T* map = nullptr;
		auto result = vertexBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapVertex");

		std::copy(std::begin(vertices), std::end(vertices), map);

		// 頂点を渡したので解放する
		vertexBuffer->Unmap(0, nullptr);

		// 頂点バッファビュー
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(vertices);
		vbView.StrideInBytes = sizeof(vertices[0]);
	}
#endif
}
