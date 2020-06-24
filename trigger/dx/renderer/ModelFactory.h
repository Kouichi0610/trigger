#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <d3d12.h>
#include "Vertex.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	class VertexShader;
	class PixelShader;
	class ITexture;

	/// <summary>
	/// IModelê∂ê¨ÉNÉâÉX
	/// </summary>
	class ModelFactory final {
	public:
		ModelFactory(ComPtr<ID3D12Device>);

		std::shared_ptr<IModel> Create(std::vector<Vertex>, std::vector<unsigned short>, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>) const;
		std::shared_ptr<IModel> Create(std::vector<TextureVertex>, std::vector<unsigned short>, ITexture*, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>) const;

		virtual ~ModelFactory() = default;
		ModelFactory(const ModelFactory&) = delete;
	private:
		ComPtr<ID3D12Device> device;
	};
}
