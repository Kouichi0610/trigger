#include "pmd.h"
#include <stdio.h>
#include <DirectXMath.h>
#include "../../logger/Logger.h"
#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"
#include "../d3dx12.h"
#include "../factory/HeapSrv.h"
#include "../renderer/texture/ITexture.h"

namespace dx {
	using namespace DirectX;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="transform"></param>
	void Pmd::Render(ComPtr<ID3D12GraphicsCommandList> commandList, const XMMATRIX& transform) {
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
		commandList->IASetVertexBuffers(0, 1, &vertexView);	// �X���b�g�ԍ��A���_�o�b�t�@�r���[�̐��AvbView
		commandList->IASetIndexBuffer(&indexView);

		commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		//commandList->DrawInstanced(vertexCount, 1, 0, 0);

		{	// TODO:�O���Ɉړ�
			static float angle = 0;
			static float cameraY = 8;
			static float cameraZ = -15;
			//auto local = XMMatrixIdentity();
			//matrix = XMMatrixScaling(2, 2, 2);

			auto world = XMMatrixRotationY(angle);

			auto eye = XMFLOAT3(0, cameraY, cameraZ);
			auto target = XMFLOAT3(0, cameraY, 0);
			auto up = XMFLOAT3(0, 1, 0);
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

			auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
				static_cast<float>(1280) / static_cast<float>(720),
				1.0f, 100.0f);


			auto matrix = world;

			const auto s = 1.0f;
			auto scale = XMMatrixScaling(s, s, s);
			matrix *= scale;


			matrix *= view;
			matrix *= projection;
			*map = matrix;	// copy.

			angle += 0.1f;
			//cameraY += 0.1f;
			//cameraZ += 0.01f;
		}
	}

	struct Header {
		float version;
		char modelName[20];
		char comment[256];
	};

	// TODO:�e�N�X�`���͖{���t�@�C������ǂݎ��͂�
	Pmd::Pmd(ComPtr<ID3D12Device> device, const char* path, ITexture* texture, std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader)
		: vertexCount(0), indexCount(0), vertexShader(vertexShader->Get()), pixelShader(pixelShader->Get()), heapSrv(nullptr) {
		FILE* fp;
		fopen_s(&fp, path, "rb");

		char signature[3];	// Pmd
		Header header;

		fread(signature, sizeof(signature), 1, fp);
		fread(&header, sizeof(header), 1, fp);
		fread(&vertexCount, sizeof(vertexCount), 1, fp);

		// read vertices
		auto totalSize = vertexCount * VertexSize;
		std::vector<unsigned char> vertices(totalSize);
		fread(vertices.data(), vertices.size(), 1, fp);

		// read indices
		fread(&indexCount, sizeof(indexCount), 1, fp);
		std::vector<unsigned short> indices(indexCount);
		fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

		fclose(fp);

		createVertices(device, vertices);
		createIndices(device, indices);
		createRootSignature(device);
		createPipelineState(device, rootSignature, vertexShader, pixelShader);

		{	//�e�N�X�`���A�萔�o�b�t�@
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
				auto size = (sizeof(XMMATRIX) + 0xff) & ~0xff;

				device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(size),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&constBuff)
				);
				constBuff->Map(0, nullptr, (void**)&map);

				auto desc = D3D12_CONSTANT_BUFFER_VIEW_DESC{};
				// TODO:constBuff
				desc.BufferLocation = constBuff->GetGPUVirtualAddress();
				desc.SizeInBytes = constBuff->GetDesc().Width;

				// �萔�o�b�t�@�r���[�̍쐬
				device->CreateConstantBufferView(&desc, handle);
			}
		}
	}

	void Pmd::createIndices(ComPtr<ID3D12Device> device, std::vector<unsigned short> indices) {
		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		indexCount = indices.size();

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = indexCount * sizeof(indices[0]);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		auto result = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)
		);
		logger::CheckError(result, "CreateIndexBuffer");

		{
			unsigned short* map = nullptr;
			auto result = indexBuffer->Map(0, nullptr, (void**)&map);
			std::copy(std::begin(indices), std::end(indices), map);
			indexBuffer->Unmap(0, nullptr);

			indexView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
			indexView.Format = DXGI_FORMAT_R16_UINT;
			indexView.SizeInBytes = indices.size() * sizeof(indices[0]);
		}
	}

	void Pmd::createVertices(ComPtr<ID3D12Device> device, std::vector<unsigned char> vertices) {
		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = vertices.size();
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		auto result = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer)
		);
		logger::CheckError(result, "Create VertexBuffer");

		{
			unsigned char* map = nullptr;
			auto result = vertexBuffer->Map(0, nullptr, (void**)&map);

			std::copy(std::begin(vertices), std::end(vertices), map);

			// ���_��n�����̂ŉ������
			vertexBuffer->Unmap(0, nullptr);

			// ���_�o�b�t�@�r���[
			vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			auto s = sizeof(vertices[0]);
			auto len = vertices.size();
			vertexView.SizeInBytes = s * len;
			vertexView.StrideInBytes = VertexSize;
		}
	}
	void Pmd::createRootSignature(ComPtr<ID3D12Device> device) {
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
		// D3D12_SHADER_VISIBILITY_ALL�ɂ���Num=2, pDescriptor��擪�A�h���X�ł��s����͂�
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
	}
	void Pmd::createPipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature, std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader) {
		D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONE_NO",  0, DXGI_FORMAT_R16G16_UINT,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHT",   0, DXGI_FORMAT_R8_UINT,         0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "EDGE_FLG", 0, DXGI_FORMAT_R8_UINT,         0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		// �O���t�B�b�N�X�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline = {};

		pipeline.pRootSignature = rootSignature.Get();

		// VS, PS�̑��Ƀh���C���V�F�[�_�A�n���V�F�[�_�A�W�I���g���V�F�[�_�Ȃǐݒ�\
		pipeline.VS.pShaderBytecode = vertexShader->Get()->GetBufferPointer();
		pipeline.VS.BytecodeLength = vertexShader->Get()->GetBufferSize();
		pipeline.PS.pShaderBytecode = pixelShader->Get()->GetBufferPointer();
		pipeline.PS.BytecodeLength = pixelShader->Get()->GetBufferSize();

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

		// 
		pipeline.DepthStencilState.DepthEnable = true;
		pipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // �������ق����̗p
		// 
		pipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
		auto result = device->CreateGraphicsPipelineState(
			&pipeline, IID_PPV_ARGS(&pipelineState));
		logger::CheckError(result, "Create PipelineState");
	}
}
