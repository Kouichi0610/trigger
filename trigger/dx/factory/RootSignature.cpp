#include "RootSignature.h"

namespace dx::factory {
	ComPtr<ID3D12RootSignature> RootSignature::Get() const {
		return rootSignature;
	}
	// TOOD:�萔�o�b�t�@�̎w��Ȃǂň���������͂�
	RootSignature::RootSignature(ComPtr<ID3D12Device> device, std::shared_ptr<logger::ILogger> logger) {
		auto desc = D3D12_ROOT_SIGNATURE_DESC{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// �o�C�i���R�[�h�̍쐬
		ComPtr<ID3DBlob> sigBlob = nullptr;
		ComPtr<ID3DBlob> error = nullptr;
		auto result = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,	// �o�[�W����
			&sigBlob,
			&error
		);
		logger->CheckError(result, error, "SerializeRootSignature");

		// �o�C�i���R�[�h�����ƂɃ��[�g�V�O�l�`�����쐬
		result = device->CreateRootSignature(
			0, // nodemask
			sigBlob->GetBufferPointer(),
			sigBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature)
		);
		logger->CheckError(result, "Create RootSignature");
		sigBlob->Release();
	}

	// SIGNATURE_VERSION��
	// device->CheckFeatureSupport();
	// �őΉ��o�[�W�������m�F�ł���
}
