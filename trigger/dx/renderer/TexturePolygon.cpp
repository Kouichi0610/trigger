#include "TexturePolygon.h"
#include "components/VertexBuffer.h"
#include "components/IndexBuffer.h"
#include "texture/ITexture.h"

#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"

#include "../factory/HeapSrv.h"

#include "../../logger/Logger.h"


namespace dx {
	using namespace DirectX;

	TexturePolygon::TexturePolygon(ComPtr<ID3D12Device> device, std::vector<TextureVertex> vertices, std::vector<unsigned short> indices, ITexture* texture, std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader) {
		vertexBuffer = std::make_unique<VertexBuffer>(device, vertices);
		indexBuffer = std::make_unique<IndexBuffer>(device, indices);

		auto heapSrv = factory::HeapSrv(device);
		this->heapSrv = heapSrv.Get();
		// �V�F�[�_�[���\�[�X�r���[�쐬
		auto desc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
		desc.Format = texture->Format();
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
		desc.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(
			texture->Get(),
			&desc,
			this->heapSrv->GetCPUDescriptorHandleForHeapStart()
		);

		vbView = vertexBuffer->GetView();
		ibView = indexBuffer->GetView();

		this->vertexShader = vertexShader->Get();
		this->pixelShader = pixelShader->Get();

		this->rootSignature = this->createRootSignature(device);
		this->pipelineState = this->createPipelineState(device, rootSignature);

		indicesCount = indices.size();

		//dx::ThrowIfFailed(INTSAFE_E_ARITHMETIC_OVERFLOW);
	}

	void TexturePolygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		// �p�C�v���C���X�e�[�g��ݒ�
		commandList->SetPipelineState(pipelineState.Get());
		// ���[�g�V�O�l�`��(TODO:SetComputeRootSignature�Ƃ̈Ⴂ)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		ID3D12DescriptorHeap* heaps[] = { heapSrv.Get(), };
		commandList->SetDescriptorHeaps(1, heaps);
		commandList->SetGraphicsRootDescriptorTable(
			0,
			heapSrv->GetGPUDescriptorHandleForHeapStart()
		);

		// �v���~�e�B�u�g�|���W(���_�̕`����@)
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&ibView);

		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
	}

	ComPtr<ID3D12RootSignature> TexturePolygon::createRootSignature(ComPtr<ID3D12Device> device) const {
		ComPtr<ID3D12RootSignature> rootSignature;

		// (�e�N�X�`����1���ݒ肷�邽�߂�)
		auto range = D3D12_DESCRIPTOR_RANGE{};
		range.NumDescriptors = 1; // �e�N�X�`��1��
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ShaderResourceView �e�N�X�`��
		range.BaseShaderRegister = 0;	// 0�ԃX���b�g����
		// �f�B�X�N���v�^�����W�͘A�����Ă���
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// params�͂����炭�z��w��\
		auto params = D3D12_ROOT_PARAMETER{};
		params.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		params.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // �s�N�Z���V�F�[�_���猩����
		params.DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRanges�̐�
		params.DescriptorTable.pDescriptorRanges = &range;

		auto desc = D3D12_ROOT_SIGNATURE_DESC{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = &params;	// TODO:���[�g�p�����[�^�擪�A�h���X
		desc.NumParameters = 1;

		// �T���v���[�̐ݒ�(�e�N�X�`������ǂ��F�����o����)
		auto sampler = D3D12_STATIC_SAMPLER_DESC{};
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // �c�����̌J��Ԃ�
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // �������̌J��Ԃ�
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // ���s�̌J��Ԃ�
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // ���`���
		sampler.MaxLOD = D3D12_FLOAT32_MAX;	// �~�b�v�}�b�v�ő�l
		sampler.MinLOD = 0.0f; // �~�b�v�}�b�v�ŏ��l
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // �s�N�Z���V�F�[�_���猩����
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // ���T���v�����O���Ȃ�+-

		desc.NumStaticSamplers = 1;
		desc.pStaticSamplers = &sampler;

		// �o�C�i���R�[�h�̍쐬
		ComPtr<ID3DBlob> sigBlob = nullptr;
		ComPtr<ID3DBlob> error = nullptr;
		auto result = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,	// �o�[�W����
			&sigBlob,
			&error
		);
		logger::CheckError(result, error, "Create Binary");

		// �o�C�i���R�[�h�����ƂɃ��[�g�V�O�l�`�����쐬
		result = device->CreateRootSignature(
			0, // nodemask
			sigBlob->GetBufferPointer(),
			sigBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature)
		);
		logger::CheckError(result, "Create RootSignature");
		sigBlob->Release();
		return rootSignature;
	}
	ComPtr<ID3D12PipelineState> TexturePolygon::createPipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature) const {
		ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },	// XYZ
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},	// UV
		};
		// �O���t�B�b�N�X�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline = {};

		pipeline.pRootSignature = rootSignature.Get();

		// VS, PS�̑��Ƀh���C���V�F�[�_�A�n���V�F�[�_�A�W�I���g���V�F�[�_�Ȃǐݒ�\
		pipeline.VS.pShaderBytecode = vertexShader->GetBufferPointer();
		pipeline.VS.BytecodeLength = vertexShader->GetBufferSize();
		pipeline.PS.pShaderBytecode = pixelShader->GetBufferPointer();
		pipeline.PS.BytecodeLength = pixelShader->GetBufferSize();

		// �T���v���}�X�N�A���X�^���C�U�[�X�e�[�g
		pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// �A���`�G�C���A�X���g���Ȃ�true
		pipeline.RasterizerState.MultisampleEnable = false;
		pipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		// �[�x�����̃N���b�s���O
		pipeline.RasterizerState.DepthClipEnable = true;

		pipeline.RasterizerState.FrontCounterClockwise = false;
		pipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		pipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipeline.RasterizerState.AntialiasedLineEnable = false;
		pipeline.RasterizerState.ForcedSampleCount = 0;
		pipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
		blendDesc.BlendEnable = false;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendDesc.LogicOpEnable = false;

		// �u�����h�X�e�[�g�̐ݒ�
		pipeline.BlendState.AlphaToCoverageEnable = false;	// ���e�X�g
		pipeline.BlendState.IndependentBlendEnable = false;	// true�ɂ����RenderTarget[8]���ꂼ��Ɋ��蓖�Ă���悤��
		pipeline.BlendState.RenderTarget[0] = blendDesc;

		// ���̓��C�A�E�g(�擪�ō쐬��������)
		pipeline.InputLayout.pInputElementDescs = inputLayouts;
		pipeline.InputLayout.NumElements = _countof(inputLayouts);

		// IBStripCutVaue
		pipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// �\���v�f(�_�A���A�O�p�`)
		pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �����_�[�^�[�Q�b�g�̐ݒ�
		pipeline.NumRenderTargets = 1;
		// @warning swapchain�̐ݒ�ƍ��킹�Ȃ��ꍇ�̋����s���B���Ԃ�G���[�o��
		pipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`1�ɐ��K������Ă���

		// �A���`�G�C���A�X�̂��߂̃T���v���ݒ�
		pipeline.SampleDesc.Count = 1;
		pipeline.SampleDesc.Quality = 0;

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
		auto result = device->CreateGraphicsPipelineState(
			&pipeline, IID_PPV_ARGS(&pipelineState));
		logger::CheckError(result, "Create PipelineState");

		return pipelineState;
	}
}
