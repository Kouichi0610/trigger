#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "ITexture.h"
#include "../Vertex.h"

namespace dx {
	using Microsoft::WRL::ComPtr;
	/// <summary>
	/// サブリソース経由のテクスチャ
	/// 
	/// CPU経由で転送？
	/// </summary>
	class TextureBuffer final : public ITexture {
	public:
		virtual ID3D12Resource* Get() const;
		virtual DXGI_FORMAT Format() const;

		TextureBuffer(ComPtr<ID3D12Device>, const wchar_t* path);
		TextureBuffer(ComPtr<ID3D12Device>, std::vector<TexRGBA>, int w, int h);

		virtual ~TextureBuffer() = default;
		TextureBuffer(const TextureBuffer&) = delete;
	private:
		ComPtr<ID3D12Resource> textureBuffer;
		DXGI_FORMAT format;
	};
}
