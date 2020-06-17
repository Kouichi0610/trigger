#include "SamplePolygon.h"
#include "../factory/PipelineState.h"
#include "ModelBase.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	ComPtr<ID3D12PipelineState> SamplePolygon::createPipelineState(ComPtr<ID3D12Device> device) {
		auto p = factory::PipelineState(device, rootSignature, vertexShader, pixelShader, logger);
		return p.Get();
	}
	void SamplePolygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		render(commandList, Topology::TriangleList, 6);
	}

	SamplePolygon::SamplePolygon() {
	}

	void SamplePolygon::Init(const ModelInfo& info) {
		ModelBase::Init(info);
		// 頂点は時計回りにすること
		std::vector<XMFLOAT3> vertices = {
			{-0.4f,-0.7f,0.0f} ,//左下
			{-0.4f,0.7f,0.0f} ,//左上
			{0.4f,-0.7f,0.0f} ,//右下
			{0.4f,0.7f,0.0f} ,//右上
		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		mapVertex(vertices);
		mapIndices(indices);
	}
}
