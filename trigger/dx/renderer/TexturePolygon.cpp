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
		// シェーダーリソースビュー作成
		auto desc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
		desc.Format = texture->Format();
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
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
		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());
		// ルートシグネチャ(TODO:SetComputeRootSignatureとの違い)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		ID3D12DescriptorHeap* heaps[] = { heapSrv.Get(), };
		commandList->SetDescriptorHeaps(1, heaps);
		commandList->SetGraphicsRootDescriptorTable(
			0,
			heapSrv->GetGPUDescriptorHandleForHeapStart()
		);

		// プリミティブトポロジ(頂点の描画方法)
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// スロット番号、頂点バッファビューの数、vbView
		commandList->IASetIndexBuffer(&ibView);

		commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
	}

	ComPtr<ID3D12RootSignature> TexturePolygon::createRootSignature(ComPtr<ID3D12Device> device) const {
		ComPtr<ID3D12RootSignature> rootSignature;

		// (テクスチャを1枚設定するための)
		auto range = D3D12_DESCRIPTOR_RANGE{};
		range.NumDescriptors = 1; // テクスチャ1つ
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ShaderResourceView テクスチャ
		range.BaseShaderRegister = 0;	// 0番スロットから
		// ディスクリプタレンジは連続している
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// paramsはおそらく配列指定可能
		auto params = D3D12_ROOT_PARAMETER{};
		params.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		params.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダから見える
		params.DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRangesの数
		params.DescriptorTable.pDescriptorRanges = &range;

		auto desc = D3D12_ROOT_SIGNATURE_DESC{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = &params;	// TODO:ルートパラメータ先頭アドレス
		desc.NumParameters = 1;

		// サンプラーの設定(テクスチャからどう色を取り出すか)
		auto sampler = D3D12_STATIC_SAMPLER_DESC{};
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 縦方向の繰り返し
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 横方向の繰り返し
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 奥行の繰り返し
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形補間
		sampler.MaxLOD = D3D12_FLOAT32_MAX;	// ミップマップ最大値
		sampler.MinLOD = 0.0f; // ミップマップ最小値
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダから見える
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // リサンプリングしない+-

		desc.NumStaticSamplers = 1;
		desc.pStaticSamplers = &sampler;

		// バイナリコードの作成
		ComPtr<ID3DBlob> sigBlob = nullptr;
		ComPtr<ID3DBlob> error = nullptr;
		auto result = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,	// バージョン
			&sigBlob,
			&error
		);
		logger::CheckError(result, error, "Create Binary");

		// バイナリコードをもとにルートシグネチャを作成
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
		logger::CheckError(result, "Create PipelineState");

		return pipelineState;
	}
}
