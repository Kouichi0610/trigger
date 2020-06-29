#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "IModel.h"
#include "Vertex.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

using namespace DirectX;

namespace dx {
	using Microsoft::WRL::ComPtr;
	class ITexture;
	class VertexBuffer;
	class IndexBuffer;
	class PixelShader;
	class VertexShader;
	class IModelData;

	class Model : public IModel {
	public:
		Model(std::shared_ptr<IModelData>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>);

		virtual ~Model() = default;
		Model(const Model&) = delete;
	private:
		std::shared_ptr<IModelData> modelData;
	};
}
