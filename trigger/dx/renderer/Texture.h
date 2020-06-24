#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;
	class CommandExecutor;
	class VertexShader;
	class PixelShader;

	/// <summary>
	/// CommandList経由でのテクスチャ作成
	/// 
	/// TODO:実装
	/// TODO:Loggerをthrowiffailed形式に
	/// 
	/// </summary>
	class Texture final {
	public:

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
	};
}
