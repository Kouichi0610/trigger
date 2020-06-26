#include <memory>
#include "renderer/IModel.h"
#include "ModelFactory.h"
#include "shader/VertexShader.h"
#include "shader/PixelShader.h"
#include "renderer/texture/ITexture.h"
#include "renderer/Polygon.h"
#include "renderer/TexturePolygon.h"
#include "renderer/IndexBuffer.h"
#include "renderer/VertexBuffer.h"

namespace dx {

	std::shared_ptr<IModel> ModelFactory::Create(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps) const {
		return std::make_shared<Polygon>(device, vertices, indices, vs, ps);
	}
	std::shared_ptr<IModel> ModelFactory::Create(
			std::vector<TextureVertex> vertices,
			std::vector<unsigned short> indices,
			ITexture* texture,
			std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps) const {
		return std::make_shared<TexturePolygon>(device, vertices, indices, texture, vs, ps);
	}

	ModelFactory::ModelFactory(ComPtr<ID3D12Device> device) {
		this->device = device;
	}
}
