#include "Model.h"
#include "../model/IModelData.h"

namespace dx {
	Model::Model(std::shared_ptr<IModelData> modelData) : modelData(modelData) {
	}

	void Model::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
		auto transform = XMMatrixIdentity();

		modelData->Render(commandList, transform);
	}
}
