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
			ID3D12Resource	テクスチャ置き場
			DescriptorHeapとは別モノ
			なのでDescriptorHeapを個別に作って問題ない

			TODO:Descriptorを定数＆テクスチャに対応
			TODO:Transform(定数バッファ作成)
			TODO:RootSignatureの作成方法検証

			TODO:これもFactory必要(renderer/pipeline と renderer/rootsignatureは削除する)

		*/
		{	// テクスチャ、定数バッファ
			auto heapSrv = factory::HeapSrv(device, 2);
			this->heapSrv = heapSrv.Get();
			// シェーダーリソースビュー作成
			auto desc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
			desc.Format = texture->Format();
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
			desc.Texture2D.MipLevels = 1;
			
			auto handle = this->heapSrv->GetCPUDescriptorHandleForHeapStart();

			// テクスチャ
			device->CreateShaderResourceView(
				texture->Get(),
				&desc,
				handle
			);

			{	// 定数バッファ
				descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				handle.ptr += descriptorHandleIncrementSize;
				constBuff = createMatrixConstBuffer(device);

#if false
				matrix.r[0].m128_f32[0] = 2.0f / 1280;
				matrix.r[1].m128_f32[1] = -2.0f / 720;
				// 並行移動
				matrix.r[3].m128_f32[0] = -1.0f;
				matrix.r[3].m128_f32[1] = 1.0f;
#endif


				constBuff->Map(0, nullptr, (void**)&map);

				auto desc = D3D12_CONSTANT_BUFFER_VIEW_DESC{};
				// TODO:constBuff
				desc.BufferLocation = constBuff->GetGPUVirtualAddress();
				desc.SizeInBytes = constBuff->GetDesc().Width;

				// 定数バッファビューの作成
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
		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());
		// ルートシグネチャ(TODO:SetComputeRootSignatureとの違い)
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		ID3D12DescriptorHeap* heaps[] = { heapSrv.Get(), };
		commandList->SetDescriptorHeaps(1, heaps);

		// ルート0 テクスチャ
		auto handle = heapSrv->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, handle);
		// ルート1 Matrix
		handle.ptr += descriptorHandleIncrementSize;
		commandList->SetGraphicsRootDescriptorTable(1, handle);


		// プリミティブトポロジ(頂点の描画方法)
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);	// スロット番号、頂点バッファビューの数、vbView
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

		// (テクスチャを1枚設定するための)
		D3D12_DESCRIPTOR_RANGE ranges[2] = {};
		{
			// テクスチャ
			ranges[0].NumDescriptors = 1; // テクスチャ1つ
			ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ShaderResourceView テクスチャ
			ranges[0].BaseShaderRegister = 0;	// 0番スロットから
			// ディスクリプタレンジは連続している
			ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			// 定数(Matrix)
			ranges[1].NumDescriptors = 1;
			ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			ranges[1].BaseShaderRegister = 0;
			ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		// parameterも複数持つ
		// TODO:NumDescriptorRanges複数設定するケース
		// たぶんD3D12_SHADER_VISIBILITY_ALLにしてNum=2, pDescriptorを先頭アドレスでも行けるはず
		D3D12_ROOT_PARAMETER params[2] = {};
		{
			params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダから見える
			params[0].DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRangesの数
			params[0].DescriptorTable.pDescriptorRanges = &ranges[0];

			params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 頂点シェーダから見える
			params[1].DescriptorTable.NumDescriptorRanges = 1;	// pDescriptorRangesの数
			params[1].DescriptorTable.pDescriptorRanges = &ranges[1];
		}

		auto desc = D3D12_ROOT_SIGNATURE_DESC{};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = params;	// TODO:ルートパラメータ先頭アドレス
		desc.NumParameters = 2;

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
