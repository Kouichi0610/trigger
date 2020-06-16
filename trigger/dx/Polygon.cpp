#include <DirectXMath.h>
#include "Polygon.h"
#include "factory/ComittedResource.h"
#include "factory/VertexShader.h"
#include "factory/PixelShader.h"
#include "factory/RootSignature.h"
#include "factory/PipelineState.h"

using namespace DirectX;

namespace dx {
	// 頂点は時計回りにすること
	XMFLOAT3 vertices[] = {
		{-0.4f,-0.7f,0.0f} ,//左下
		{-0.4f,0.7f,0.0f} ,//左上
		{0.4f,-0.7f,0.0f} ,//右下
		{0.4f,0.7f,0.0f} ,//右上
	};
	unsigned short indices[] = { 0,1,2, 2,1,3 };

	void Polygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());
		// ルートシグネチャ(TODO:SetComputeRootSignatureとの違い)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// プリミティブトポロジ(頂点の描画方法)
		auto topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		//topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// スロット番号、頂点バッファビューの数、vbView
		commandList->IASetIndexBuffer(&ibView);

		// TODO:indexBuffer
		// 描画命令(TODO:引数詳細)
		// 頂点数、インスタンス数、頂点データオフセット、インスタンスオフセット
		// インスタンス数 プリミティブ表示数
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

	Polygon::Polygon(ComPtr<ID3D12Device> device, std::shared_ptr<logger::ILogger> logger) {
		auto vb = factory::ComittedResource(device, sizeof(vertices), logger);
		auto ib = factory::ComittedResource(device, sizeof(indices), logger);
		auto vs = factory::VertexShader(L"BasicVertexShader.hlsl", "BasicVS", logger);
		auto ps = factory::PixelShader(L"BasicPixelShader.hlsl", "BasicPS", logger);
		auto rootSig = factory::RootSignature(device, logger);

		vertexBuffer = vb.Get();
		indicesBuffer = ib.Get();

		vertexShader = vs.Get();
		pixelShader = ps.Get();

		rootSignature = rootSig.Get();

		auto pipeline = factory::PipelineState(device, rootSignature, vertexShader, pixelShader, logger);
		pipelineState = pipeline.Get();

		MapVertex(logger);
		MapIndices(logger);
	}
	void Polygon::MapVertex(std::shared_ptr<logger::ILogger> logger) {
		XMFLOAT3* map = nullptr;
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
	void Polygon::MapIndices(std::shared_ptr<logger::ILogger> logger) {
		//作ったバッファにインデックスデータをコピー
		unsigned short* map = nullptr;
		auto result = indicesBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapIndices");

		std::copy(std::begin(indices), std::end(indices), map);

		indicesBuffer->Unmap(0, nullptr);

		//インデックスバッファビューを作成
		ibView.BufferLocation = indicesBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeof(indices);
	}
}
