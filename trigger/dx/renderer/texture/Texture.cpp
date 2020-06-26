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
	/// CPU��				�}�b�v	GPU��				�R�s�[
	/// �e�N�X�`���f�[�^	->		�A�b�v���[�h�o�b�t�@ -> �ǂݎ��p�o�b�t�@
	///														(������e�N�X�`���Ƃ��ė��p����)
	/// 
	/// </summary>
	/// <param name="path"></param>
	void Texture::Load(const wchar_t* path) {
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		auto result = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
		auto img = scratchImg.GetImage(0, 0, 0);

		format = metadata.format;

		// ����256�ɍ��킹��
		auto pitch = img->rowPitch + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - img->rowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

		{	// �A�b�v���[�h�o�b�t�@(GPU��)
			auto prop = D3D12_HEAP_PROPERTIES{};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// map��L����
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			// �P��A�_�v�^�[
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
				D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����`�����݉\�AGPU�͓ǂݎ��̂�
				nullptr,
				IID_PPV_ARGS(&uploadBuffer)
			);
			logger::CheckError(result, "UploadBuffer");
		}
		{	// �R�s�[��o�b�t�@�쐬
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
				D3D12_RESOURCE_STATE_COPY_DEST,	// �R�s�[��
				nullptr,
				IID_PPV_ARGS(&textureBuffer)
			);
			logger::CheckError(result, "Copy TextureBuffer");
		}
		{
			// �A�b�v���[�h�o�b�t�@�ւ�map
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

		{	// �A�b�v���[�h�o�b�t�@����ǂݎ��o�b�t�@�ւ̓]��
			auto src = D3D12_TEXTURE_COPY_LOCATION{};
			src.pResource = uploadBuffer.Get();
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // pResource�Ƀo�b�t�@���\�[�X(SUBRESOURCE_INDEX�Ńe�N�X�`�����\�[�X)
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
		// �o���A�ƃt�F���X�̐ݒ�
		// GPU����̂��߁A�t�F���X�ŏI����҂�
		// TODO:���\�[�X�o���A�ɂ���
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

			// �R�}���h���X�g���s
			executor->Execute(commandList);
		}


	}
}