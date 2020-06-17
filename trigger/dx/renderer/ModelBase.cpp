#include "ModelBase.h"
#include "ModelInfo.h"
#include "../factory/ComittedResource.h"
#include "../factory/VertexShader.h"
#include "../factory/PixelShader.h"
#include "../factory/RootSignature.h"

namespace dx {
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="commandList"></param>
	void ModelBase::render(ComPtr<ID3D12GraphicsCommandList> commandList, Topology topology, int indicesCount) const {
		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());
		// ルートシグネチャ(TODO:SetComputeRootSignatureとの違い)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// プリミティブトポロジ(頂点の描画方法)
		commandList->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
		commandList->IASetVertexBuffers(0, 1, &vbView);	// スロット番号、頂点バッファビューの数、vbView
		commandList->IASetIndexBuffer(&ibView);

		// TODO:indexBuffer
		// 描画命令(TODO:引数詳細)
		// 頂点数、インスタンス数、頂点データオフセット、インスタンスオフセット
		// インスタンス数 プリミティブ表示数
		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
	}

	void ModelBase::mapIndices(std::vector<unsigned short> indices) {
		unsigned short* map = nullptr;
		auto result = indicesBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapIndices");
		std::copy(std::begin(indices), std::end(indices), map);
		indicesBuffer->Unmap(0, nullptr);

		ibView.BufferLocation = indicesBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeof(indices);
	}
	void ModelBase::mapIndices(std::vector<unsigned int> indices) {
		unsigned int* map = nullptr;
		auto result = indicesBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapIndices");
		std::copy(std::begin(indices), std::end(indices), map);
		indicesBuffer->Unmap(0, nullptr);

		ibView.BufferLocation = indicesBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = sizeof(indices);
	}

	void ModelBase::Init(const ModelInfo& info) {
		this->logger = info.GetLogger();
		auto device = info.GetDevice();
		auto vb = factory::ComittedResource(device, info.VerticesSize, logger);
		auto ib = factory::ComittedResource(device, info.IndicesSize, logger);
		auto vs = factory::VertexShader(info.VertexShaderFile, info.VertexShaderEntry, logger);
		auto ps = factory::PixelShader(info.PixelShaderFile, info.PixelShaderEntry, logger);
		auto rootSig = factory::RootSignature(device, logger);

		vertexBuffer = vb.Get();
		indicesBuffer = ib.Get();
		vertexShader = vs.Get();
		pixelShader = ps.Get();
		rootSignature = rootSig.Get();

		pipelineState = createPipelineState(device);
	}
	ModelBase::ModelBase() {
	}
}