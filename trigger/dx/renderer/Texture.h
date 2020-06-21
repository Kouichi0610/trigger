#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;
	class VertexShader;
	class PixelShader;

	/// <summary>
	/// CommandList�o�R�ł̃e�N�X�`���쐬
	/// 
	/// TODO:����
	/// TODO:Logger��throwiffailed�`����
	/// 
	/// </summary>
	class Texture final {
	public:

		Texture(ComPtr<ID3D12Device>, ComPtr<ID3D12GraphicsCommandList>, ComPtr<ID3D12CommandQueue>);
		void Load(const wchar_t* texturePath);

		virtual ~Texture() = default;
		Texture(const Texture&) = delete;
	private:
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ComPtr<ID3D12CommandQueue> commandQueue;

		ComPtr<ID3D12Resource> uploadBuffer;
		ComPtr<ID3D12Resource> textureBuffer;
	};
}
