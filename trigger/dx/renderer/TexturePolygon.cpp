#include "TexturePolygon.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "texture/ITexture.h"

#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"

#include "../factory/HeapSrv.h"

#include "../../logger/Logger.h"

#include "../d3dx12.h"


namespace dx {
	using namespace DirectX;
	ComPtr<ID3D12Resource> TexturePolygon::createMatrixConstBuffer(ComPtr<ID3D12Device> device) const {
		ComPtr<ID3D12Resource> buffer;
		auto size = (sizeof(XMMATRIX) + 0xff) & ~0xff;

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&buffer)
		);
		return buffer;
	}

	TexturePolygon::TexturePolygon(ComPtr<ID3D12Device> device, std::vector<TextureVertex> vertices, std::vector<unsigned short> indices, ITexture* texture, std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader) {
		vertexBuffer = std::make_unique<VertexBuffer>(device, vertices);
		indexBuffer = std::make_unique<IndexBuffer>(device, indices);

		/*
			ID3D12Resource	�e�N�X�`���u����
			DescriptorHeap�Ƃ͕ʃ��m
			�Ȃ̂�DescriptorHeap���ʂɍ���Ė��Ȃ�

			TODO:Descriptor��萔���e�N�X�`���ɑΉ�
			TODO:Transform(�萔�o�b�t�@�쐬)
			TODO:RootSignature�̍쐬���@����

			TODO:�����Factory�K�v(renderer/pipeline �� renderer/rootsignature�͍폜����)

		*/
		{	// �e�N�X�`���A�萔�o�b�t�@
			auto heapSrv = factory::HeapSrv(device, 2);
			this->heapSrv = heapSrv.Get();
			// �V�F�[�_�[���\�[�X�r���[�쐬
			auto desc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
			desc.Format = texture->Format();
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
			desc.Texture2D.MipLevels = 1;
			
			auto handle = this->heapSrv->GetCPUDescriptorHandleForHeapStart();

			// �e�N�X�`��
			device->CreateShaderResourceView(
				texture->Get(),
				&desc,
				handle
			);

			{	// �萔�o�b�t�@
				descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				handle.ptr += descriptorHandleIncrementSize;
				constBuff = createMatrixConstBuffer(device);

#if false
				matrix.r[0].m128_f32[0] = 2.0f / 1280;
				matrix.r[1].m128_f32[1] = -2.0f / 720;
				// ���s�ړ�
				matrix.r[3].m128_f32[0] = -1.0f;
				matrix.r[3].m128_f32[1] = 1.0f;
#endif


				constBuff->Map(0, nullptr, (void**)&map);

				auto desc = D3D12_CONSTANT_BUFFER_VIEW_DESC{};
				// TODO:constBuff
				desc.BufferLocation = constBuff->GetGPUVirtualAddress();
				desc.SizeInBytes = constBuff->GetDesc().Width;

				// �萔�o�b�t�@�r���[�̍쐬
				device->CreateConstantBufferView(&desc, handle);
			}
		}

		vbView = vertexBuffer->GetView();
		ibView = indexBuffer->GetView();

		this->vertexShader = vertexShader->Get();
		this->pixelShader = pixelShader->Get();

		this->rootSignature = this->createRootSignature(device);
		this->pipelineState = this->createPipelineState(device, rootSignature);

		indicesCount = indices.size();
	}

	void TexturePolygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		// �p�C�v���C���X�e�[�g��ݒ�
		commandList->SetPipelineState(pipelineState.Get());
		// ���[�g�V�O�l�`��(TODO:SetComputeRootSignature�Ƃ̈Ⴂ)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		ID3D12DescriptorHeap* heaps[] = { heapSrv.Get(), };
		commandList->SetDescriptorHeaps(1, heaps);

		// ���[�g0 �e�N�X�`��
		auto handle = heapSrv->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, handle);
		// ���[�g1 Matrix
		handle.ptr += descriptorHandleIncrementSize;
		commandList->SetGraphicsRootDescriptorTable(1, handle);


		// �v���~�e�B�u�g�|���W(���_�̕`����@)
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&ibView);

		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);

		{
			static float angle = 0;
			static float cameraY = 0;
			static float cameraZ = -5;
			//auto local = XMMatrixIdentity();
			//matrix = XMMatrixScaling(2, 2, 2);

			auto world = XMMatrixRotationY(angle);

			auto eye = XMFLOAT3(0, cameraY, cameraZ);
			auto target = XMFLOAT3(0, 0, 0);
			auto up = XMFLOAT3(0, 1, 0);
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

			auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
				static_cast<float>(1280) / static_cast<float>(720),
				1.0f, 10.0f);


			auto matrix = world;
			matrix *= view;
			matrix *= projection;
			*map = matrix;	// copy.

			angle += 0.1f;
			//cameraY += 0.1f;
			//cameraZ += 0.01f;
		}
	}

	ComPtr<ID3D12RootSignature> TexturePolygon::createRootSignature(ComPtr<ID3D12Device> device) const {
		ComPtr<ID3D12RootSignature> rootSignature;

		// (�e�N�X�`����1���ݒ肷�邽�߂�)
		D3D12_DESCRIPTOR_RANGE ranges[2] = {};
		{
			// �e�N�X�`��
			ranges[0].NumDescriptors = 1; // �e�N�X�`��1��
			ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ShaderResourceView �e�N�X�`��
			ranges[0].BaseShaderRegister = 0;	// 0�ԃX���b�g����
			// �f�B�X�N���v�^�����W�͘A�����Ă���
			ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			// �萔(Matrix)
			ranges[1].NumDescriptors = 1;
			ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			ranges[1].BaseShaderRegister = 0;
			ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		// parameter����������
		// TODO:NumDescriptorRanges�����ݒ肷��P�[�X
		// ���Ԃ�D3D12_SHADER_VISIBILITY_ALL�ɂ���Num=2, pDescriptor��擪�A�h���X�ł��s����͂�
		D3D12_ROOT_PARAMETER params[2] = {};
		{
			params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // �s�N�Z���V�F�[�_���猩����
			params[0].DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRanges�̐�
			params[0].DescriptorTable.pDescriptorRanges = &ranges[0];

			params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // ���_�V�F�[�_���猩����
			params[1].DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRanges�̐�
			params[1].DescriptorTable.pDescriptorRanges = &ranges[1];
		}

		auto desc = D3D12_ROOT_SIGNATURE_DESC{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = params;	// TODO:���[�g�p�����[�^�擪�A�h���X
		desc.NumParameters = 2;

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
