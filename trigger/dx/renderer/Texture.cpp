#include <DirectXTex.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "Texture.h"
#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"
#include "../../logger/Logger.h"

using namespace DirectX;

namespace dx {
	Texture::Texture(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> list, ComPtr<ID3D12CommandQueue> queue) {
		this->device = device;
		this->commandList = list;
		this->commandQueue = queue;
	}
	void Texture::Load(const wchar_t* path) {
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		auto result = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
		auto img = scratchImg.GetImage(0, 0, 0);

		{	// アップロードバッファ(GPU側)
			auto prop = D3D12_HEAP_PROPERTIES{};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// mapを有効に
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			// 単一アダプター
			prop.CreationNodeMask = 0;
			prop.VisibleNodeMask = 0;

			auto desc = D3D12_RESOURCE_DESC{};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Width = img->slicePitch;
			desc.Height = 1;
			// TODO:TextureBuffer
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			auto result = device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUから描きこみ可能、GPUは読み取りのみ
				nullptr,
				IID_PPV_ARGS(&uploadBuffer)
			);
			logger::CheckError(result, "UploadBuffer");
		}
		{	// コピー先バッファ作成
			auto prop = D3D12_HEAP_PROPERTIES{};
			prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
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

			auto result = device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,	// コピー先
				nullptr,
				IID_PPV_ARGS(&textureBuffer)
			);
			logger::CheckError(result, "Copy TextureBuffer");

			//TODO:map
		}

	}
}