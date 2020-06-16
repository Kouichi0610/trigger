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
		// グラフィックスパイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline = {};

		pipeline.pRootSignature = rootSignature.Get();

		// VS, PSの他にドメインシェーダ、ハルシェーダ、ジオメトリシェーダなど設定可能
		pipeline.VS.pShaderBytecode = vertexShader->GetBufferPointer();
		pipeline.VS.BytecodeLength = vertexShader->GetBufferSize();
		pipeline.PS.pShaderBytecode = pixelShader->GetBufferPointer();
		pipeline.PS.BytecodeLength = pixelShader->GetBufferSize();

		// サンプルマスク、ラスタライザーステート
		pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// アンチエイリアスを使うならtrue
		pipeline.RasterizerState.MultisampleEnable = false;
		pipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		// 深度方向のクリッピング
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

		// ブレンドステートの設定
		pipeline.BlendState.AlphaToCoverageEnable = false;	// αテスト
		pipeline.BlendState.IndependentBlendEnable = false;	// trueにするとRenderTarget[8]それぞれに割り当てられるように
		pipeline.BlendState.RenderTarget[0] = blendDesc;

		// 入力レイアウト(先頭で作成したもの)
		pipeline.InputLayout.pInputElementDescs = inputLayouts;
		pipeline.InputLayout.NumElements = _countof(inputLayouts);

		// IBStripCutVaue
		pipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// 構成要素(点、線、三角形)
		pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// レンダーターゲットの設定
		pipeline.NumRenderTargets = 1;
		// @warning swapchainの設定と合わせない場合の挙動不明。たぶんエラー出る
		pipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～1に正規化されている

		// アンチエイリアスのためのサンプル設定
		pipeline.SampleDesc.Count = 1;
		pipeline.SampleDesc.Quality = 0;

		// グラフィックスパイプラインステートオブジェクトの作成
		auto result = device->CreateGraphicsPipelineState(
			&pipeline, IID_PPV_ARGS(&pipelineState));

		logger->CheckError(result, "Create PipelineState");
	}
}