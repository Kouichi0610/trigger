#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>

namespace dx {
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	/// <summary>
	/// モデルデータインターフェイス
	/// TODO:名前(IModel含め)
	/// </summary>
	class IModelData {
	public:
		virtual void Render(ComPtr<ID3D12GraphicsCommandList>, const XMMATRIX&) = 0;

		IModelData() = default;
		IModelData(const IModelData&) = delete;
		virtual ~IModelData() = default;
	};
}
