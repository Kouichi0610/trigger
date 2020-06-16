#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../logger/ILogger.h"
#include <DirectXMath.h>

using namespace DirectX;
namespace dx {
	using Microsoft::WRL::ComPtr;

	/// <summary>
	/// ポリゴン描画試作
	/// </summary>
	class Polygon {
	public:
		struct Vertex {
			XMFLOAT3 pos;
			XMFLOAT2 uv;
		};
		void Render(ComPtr<ID3D12GraphicsCommandList>);

		Polygon(ComPtr<ID3D12Device>, std::shared_ptr<logger::ILogger>);
		Polygon(const Polygon&) = delete;
	private:

		ComPtr<ID3D12Resource> vertexBuffer;
		ComPtr<ID3D12Resource> indicesBuffer;
		ComPtr<ID3D12Resource> constantBuffer;

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

		ComPtr<ID3D12RootSignature> rootSignature;
		// 違うパラメータを設定したい場合、作り直す
		ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_INDEX_BUFFER_VIEW ibView;
		D3D12_VERTEX_BUFFER_VIEW vbView;

		void MapVertex(std::shared_ptr<logger::ILogger>);
		void MapIndices(std::shared_ptr<logger::ILogger>);
	};
}
