#include "IModel.h"
#include "ModelFactory.h"
#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"
#include "Polygon.h"
#include "TexturePolygon.h"

namespace dx {

	std::shared_ptr<IModel> ModelFactory::Create(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps) const {
		return std::make_shared<Polygon>(device, vertices, indices, vs, ps);
	}
	std::shared_ptr<IModel> ModelFactory::Create(std::vector<TextureVertex> vertices, std::vector<unsigned short> indices, std::vector<TexRGBA> texData, int w, int h, std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps) const {
		return std::make_shared<TexturePolygon>(device, vertices, indices, texData, w, h, vs, ps);
	}

	ModelFactory::ModelFactory(ComPtr<ID3D12Device> device) {
		this->device = device;
	}
}
