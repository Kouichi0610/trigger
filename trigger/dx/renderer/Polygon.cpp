#include "Polygon.h"
#include "components/VertexBuffer.h"
#include "components/IndexBuffer.h"
#include "rootsignature/RootSignature.h"
#include "pipeline/PipelineState.h"

#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"

namespace dx {
	Polygon::Polygon(ComPtr<ID3D12Device> device, std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader) {
		vertexBuffer = std::make_unique<VertexBuffer>(device, vertices);
		indexBuffer = std::make_unique<IndexBuffer>(device, indices);

		vbView = vertexBuffer->GetView();
		ibView = indexBuffer->GetView();

		this->vertexShader = vertexShader->Get();
		this->pixelShader = pixelShader->Get();

		auto rootSignature = RootSignature(device);
		auto pipeline = PipelineState(device, rootSignature.Get(), this->vertexShader, this->pixelShader);
		
		this->rootSignature = rootSignature.Get();
		this->pipelineState = pipeline.Get();

		indicesCount = indices.size();
	}

	void Polygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		// �p�C�v���C���X�e�[�g��ݒ�
		commandList->SetPipelineState(pipelineState.Get());
		// ���[�g�V�O�l�`��(TODO:SetComputeRootSignature�Ƃ̈Ⴂ)
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		// �v���~�e�B�u�g�|���W(���_�̕`����@)
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&ibView);

		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
	}
}
