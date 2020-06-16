#include <DirectXMath.h>
#include "Polygon.h"
#include "factory/ComittedResource.h"
#include "factory/VertexShader.h"
#include "factory/PixelShader.h"
#include "factory/RootSignature.h"
#include "factory/PipelineState.h"

using namespace DirectX;

namespace dx {
	// ���_�͎��v���ɂ��邱��
	XMFLOAT3 vertices[] = {
		{-0.4f,-0.7f,0.0f} ,//����
		{-0.4f,0.7f,0.0f} ,//����
		{0.4f,-0.7f,0.0f} ,//�E��
		{0.4f,0.7f,0.0f} ,//�E��
	};
	unsigned short indices[] = { 0,1,2, 2,1,3 };

	void Polygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		// �p�C�v���C���X�e�[�g��ݒ�
		commandList->SetPipelineState(pipelineState.Get());
		// ���[�g�V�O�l�`��(TODO:SetComputeRootSignature�Ƃ̈Ⴂ)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// �v���~�e�B�u�g�|���W(���_�̕`����@)
		auto topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		//topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&ibView);

		// TODO:indexBuffer
		// �`�施��(TODO:�����ڍ�)
		// ���_���A�C���X�^���X���A���_�f�[�^�I�t�Z�b�g�A�C���X�^���X�I�t�Z�b�g
		// �C���X�^���X�� �v���~�e�B�u�\����
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

		// ���_��n�����̂ŉ������
		vertexBuffer->Unmap(0, nullptr);

		// ���_�o�b�t�@�r���[
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(vertices);
		vbView.StrideInBytes = sizeof(vertices[0]);
	}
	void Polygon::MapIndices(std::shared_ptr<logger::ILogger> logger) {
		//������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
		unsigned short* map = nullptr;
		auto result = indicesBuffer->Map(0, nullptr, (void**)&map);
		logger->CheckError(result, "MapIndices");

		std::copy(std::begin(indices), std::end(indices), map);

		indicesBuffer->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[���쐬
		ibView.BufferLocation = indicesBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeof(indices);
	}
}
