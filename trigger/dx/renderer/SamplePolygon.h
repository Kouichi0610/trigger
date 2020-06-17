#pragma once
#include "ModelBase.h"

namespace dx {

	class SamplePolygon final : public ModelBase {
	public:
		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);
		SamplePolygon();
		virtual void Init(const ModelInfo&);
	protected:
		virtual ComPtr<ID3D12PipelineState> createPipelineState(ComPtr<ID3D12Device>);
	};

}
