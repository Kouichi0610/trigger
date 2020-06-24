#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "ITexture.h"

namespace dx {
	using Microsoft::WRL::ComPtr;
	class CommandExecutor;
	class VertexShader;
	class PixelShader;

	/// <summary>
	/// CommandList経由でのテクスチャ作成
	/// 
	/// TODO:
	/// 3.TexturePolygonにITextureを渡す形式で
	/// 
	/// 4.命名
	/// 5.
	/// 
	/// </summary>
	class Texture final : public ITexture {
	public:
		virtual ID3D12Resource* Get() const;
		virtual DXGI_FORMAT Format() const;

		Texture(ComPtr<ID3D12Device>, ComPtr<ID3D12GraphicsCommandList>, CommandExecutor*);
		void Load(const wchar_t* texturePath);

		virtual ~Texture() = default;
		Texture(const Texture&) = delete;
	private:
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		CommandExecutor* executor;

		ComPtr<ID3D12Resource> uploadBuffer;
		ComPtr<ID3D12Resource> textureBuffer;
		DXGI_FORMAT format;
	};
}
