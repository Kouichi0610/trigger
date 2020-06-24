#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace dx {
	/// <summary>
	/// テクスチャインターフェイス
	/// </summary>
	class ITexture {
	public:
		virtual ID3D12Resource* Get() const = 0;
		virtual DXGI_FORMAT Format() const = 0;

		ITexture() = default;
		virtual ~ITexture() = default;
		ITexture(const ITexture&) = delete;
	};
}
