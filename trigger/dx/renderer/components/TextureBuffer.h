#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../Vertex.h"

namespace dx {
	using Microsoft::WRL::ComPtr;
	class TextureBuffer final {
	public:
		ComPtr<ID3D12Resource> Get() const;
		TextureBuffer(ComPtr<ID3D12Device>, std::vector<TexRGBA>, int w, int h);

		virtual ~TextureBuffer() = default;
		TextureBuffer(const TextureBuffer&) = delete;
	private:
		ComPtr<ID3D12Resource> textureBuffer;
	};
}
