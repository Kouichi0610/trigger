#include <DirectXTex.h>
#include "TextureBuffer.h"

/*
	DirectXTex使い方
	1.GithubからDirectXTexをクローン
	2.適切なプロジェクトファイルからビルド
	3.DirectXTex.h, DirectXTex.lib, DirectXTex.inlをプロジェクトに持ってくる

	最初に
	CoInitializeEx(0, COINIT_MULTITHREADED);
	を呼んでおくこと
*/
/*
TODO:ミップマップ
	テクスチャ配列を使用する場合
	スライス(3Dテクスチャ)

*/
namespace dx {
	ComPtr<ID3D12Resource> TextureBuffer::Get() const {
		return textureBuffer;
	}
	DXGI_FORMAT TextureBuffer::Format() const {
		return format;
	}
	TextureBuffer::TextureBuffer(ComPtr<ID3D12Device> device, const wchar_t* path) {
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		auto result = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
		auto img = scratchImg.GetImage(0, 0, 0);

		format = metadata.format;

		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;	// 特殊設定
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	// 転送をCPUから直接行う
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Format = metadata.format;
		desc.Width = metadata.width;
		desc.Height = metadata.height;
		desc.DepthOrArraySize = metadata.arraySize;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = metadata.mipLevels;
		desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		result = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textureBuffer)
		);
		result = textureBuffer->WriteToSubresource(
			0,
			nullptr,	// 全領域へコピー
			img->pixels,
			img->rowPitch,	// 1ライン当たりのサイズ
			img->slicePitch	// 全サイズ
		);
	}
	TextureBuffer::TextureBuffer(ComPtr<ID3D12Device> device, std::vector<TexRGBA> data, int width, int height) {
		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;	// 特殊設定
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	// 転送をCPUから直接行う
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto result = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textureBuffer)
		);
		result = textureBuffer->WriteToSubresource(
			0,
			nullptr,	// 全領域へコピー
			data.data(),
			sizeof(TexRGBA) * width,	// 1ライン当たりのサイズ
			sizeof(TexRGBA) * data.size()	// 全サイズ
		);
	}
}