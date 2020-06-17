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
		// ���_�͎��v���ɂ��邱��
		std::vector<XMFLOAT3> vertices = {
			{-0.4f,-0.7f,0.0f} ,//����
			{-0.4f,0.7f,0.0f} ,//����
			{0.4f,-0.7f,0.0f} ,//�E��
			{0.4f,0.7f,0.0f} ,//�E��
		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		mapVertex(vertices);
		mapIndices(indices);
	}
}
