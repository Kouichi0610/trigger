#include "PipelineState.h"

namespace dx::factory {
	ComPtr<ID3D12PipelineState> PipelineState::Get() const {
		return pipelineState;
	}

	PipelineState::PipelineState(
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12RootSignature> rootSignature,
		ComPtr<ID3DBlob> vertexShader,
		ComPtr<ID3DBlob> pixelShader,
		std::shared_ptr<logger::ILogger> logger) {

		D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
			{
				"POSITION", 0,
				DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0,
			},
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

		logger->CheckError(result, "Create PipelineState");
	}
}