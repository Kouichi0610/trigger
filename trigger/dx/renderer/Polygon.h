#pragma once
#include "ModelBase.h"

namespace dx {
	class Polygon final : public ModelBase {
	public:
		struct Vertex {
			XMFLOAT3 pos;
			XMFLOAT2 uv;
		};

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);
		Polygon();
		
		void Init(const ModelInfo& info, std::vector<Vertex>, std::vector<unsigned short>);
	protected:
		virtual ComPtr<ID3D12PipelineState> createPipelineState(ComPtr<ID3D12Device>);
	};

}
