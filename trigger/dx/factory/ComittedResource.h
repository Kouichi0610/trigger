#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	/// <summary>
	/// VertexBuffer
	/// IndexBuffer
	/// ConstantBuffer
	/// オブジェクトの情報を描きこむためのバッファ
	/// </summary>
	class ComittedResource final {
	public:
		ComPtr<ID3D12Resource> Get() const;
		ComittedResource(ComPtr<ID3D12Device>, UINT64);

		virtual ~ComittedResource() = default;
		ComittedResource(const ComittedResource&) = delete;
	private:
		ComPtr<ID3D12Resource> buffer;
	};

}
