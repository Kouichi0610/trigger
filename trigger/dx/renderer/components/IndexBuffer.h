#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx {
	using Microsoft::WRL::ComPtr;

	class IndexBuffer final {
	public:
		IndexBuffer(ComPtr<ID3D12Device>, std::vector<unsigned short> indices);

		D3D12_INDEX_BUFFER_VIEW GetView() const;
		ComPtr<ID3D12Resource> GetIndexBuffer() const;
		int IndexCount() const;

		virtual ~IndexBuffer() = default;
		IndexBuffer(const IndexBuffer&) = delete;
	private:
		ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW view;
		int indexCount;
	};
}
