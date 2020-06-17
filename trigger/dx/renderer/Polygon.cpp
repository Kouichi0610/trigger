#include "Polygon.h"
#include "../factory/PipelineState.h"
#include "ModelBase.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	ComPtr<ID3D12PipelineState> Polygon::createPipelineState(ComPtr<ID3D12Device> device) {
		auto p = factory::PipelineState(device, rootSignature, vertexShader, pixelShader, logger);
		return p.Get();
	}
	void Polygon::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		render(commandList, Topology::TriangleList, 6);
	}

	Polygon::Polygon() {
	}

	void Polygon::Init(const ModelInfo& info, std::vector<Vertex> vertices, std::vector<unsigned short> indices) {
		ModelBase::Init(info);
		mapVertex(vertices);
		mapIndices(indices);
	}
}
