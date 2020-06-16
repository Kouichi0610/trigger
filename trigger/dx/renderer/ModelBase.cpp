#include "ModelBase.h"
#include "../factory/ComittedResource.h"
#include "../factory/VertexShader.h"
#include "../factory/PixelShader.h"
#include "../factory/RootSignature.h"

namespace dx {
	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="commandList"></param>
	void ModelBase::render(ComPtr<ID3D12GraphicsCommandList> commandList, Topology topology, int indicesCount) const {
		// �p�C�v���C���X�e�[�g��ݒ�
		commandList->SetPipelineState(pipelineState.Get());
		// ���[�g�V�O�l�`��(TODO:SetComputeRootSignature�Ƃ̈Ⴂ)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// �v���~�e�B�u�g�|���W(���_�̕`����@)
		commandList->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
		commandList->IASetVertexBuffers(0, 1, &vbView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&ibView);

		printf("t %d %d\n", vbView.SizeInBytes, ibView.SizeInBytes);
		// TODO:indexBuffer
		// �`�施��(TODO:�����ڍ�)
		// ���_���A�C���X�^���X���A���_�f�[�^�I�t�Z�b�g�A�C���X�^���X�I�t�Z�b�g
		// �C���X�^���X�� �v���~�e�B�u�\����
		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
	}
	void ModelBase::mapVertex(std::vector<XMFLOAT3> vertices) {
		XMFLOAT3* map = nullptr;
		auto result = vertexBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapVertex");

		std::copy(std::begin(vertices), std::end(vertices), map);

		// ���_��n�����̂ŉ������
		vertexBuffer->Unmap(0, nullptr);

		// ���_�o�b�t�@�r���[
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(vertices[0]) * vertices.size();
		vbView.StrideInBytes = sizeof(vertices[0]);
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

	void ModelBase::Init(ComPtr<ID3D12Device> device, const ModelInfo& info, std::shared_ptr<logger::ILogger> logger) {
		this->logger = logger;
		ModelInfo m;
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