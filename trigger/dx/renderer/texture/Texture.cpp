#include <DirectXTex.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "Texture.h"
#include "../../CommandExecutor.h"
#include "../../shader/VertexShader.h"
#include "../../shader/PixelShader.h"
#include "../../../logger/Logger.h"

using namespace DirectX;

namespace dx {
	ID3D12Resource* Texture::Get() const {
		return textureBuffer.Get();
	}
	DXGI_FORMAT Texture::Format() const {
		return format;
	}

	Texture::Texture(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> list, CommandExecutor* executor) {
		this->device = device;
		this->commandList = list;
		this->executor = executor;
	}
	/// <summary>
	/// 
	/// 
	/// CPU側				マップ	GPU側				コピー
	/// テクスチャデータ	->		アップロードバッファ -> 読み取り用バッファ
	///														(これをテクスチャとして利用する)
	/// 
	/// </summary>
	/// <param name="path"></param>
	void Texture::Load(const wchar_t* path) {
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		auto result = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
		auto img = scratchImg.GetImage(0, 0, 0);

		format = metadata.format;

		// 幅を256に合わせる
		auto pitch = img->rowPitch + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - img->rowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

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
			desc.Width = pitch * img->height;
			desc.Height = 1;
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
		}
		{
			// アップロードバッファへのmap
			uint8_t* mapforimg = nullptr;
			result = uploadBuffer->Map(0, nullptr, (void**)&mapforimg);
			logger::CheckError(result, "map to uploadBuffer.");

			auto pSrc = img->pixels;
			for (auto y = 0; y < img->height; y++) {
				std::copy_n(pSrc, pitch, mapforimg);
				pSrc += img->rowPitch;
				mapforimg += pitch;
			}
			uploadBuffer->Unmap(0, nullptr);
		}

		{	// アップロードバッファから読み取りバッファへの転送
			auto src = D3D12_TEXTURE_COPY_LOCATION{};
			src.pResource = uploadBuffer.Get();
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // pResourceにバッファリソース(SUBRESOURCE_INDEXでテクスチャリソース)
			src.PlacedFootprint.Offset = 0;
			src.PlacedFootprint.Footprint.Width = metadata.width;
			src.PlacedFootprint.Footprint.Height = metadata.height;
			src.PlacedFootprint.Footprint.Depth = metadata.depth;

			src.PlacedFootprint.Footprint.RowPitch = pitch;
			src.PlacedFootprint.Footprint.Format = img->format;

			auto dst = D3D12_TEXTURE_COPY_LOCATION{};
			dst.pResource = textureBuffer.Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;

			commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}
		// バリアとフェンスの設定
		// GPU操作のため、フェンスで終了を待つ
		// TODO:リソースバリアについて
		{
			auto desc = D3D12_RESOURCE_BARRIER{};
			desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			desc.Transition.pResource = textureBuffer.Get();
			desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			desc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			commandList->ResourceBarrier(1, &desc);
			commandList->Close();

			// コマンドリスト実行
			executor->Execute(commandList);
		}


	}
}