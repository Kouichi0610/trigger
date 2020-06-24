#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class IModel {
	public:
		virtual void Render(ComPtr<ID3D12GraphicsCommandList>) = 0;

		IModel() = default;
		virtual ~IModel() = default;

		IModel(const IModel&) = delete;
	};

}
