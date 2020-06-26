#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../../logger/Logger.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	class VertexBuffer {
	public:
		template <typename T>
		VertexBuffer(ComPtr<ID3D12Device>, std::vector<T> vertices);

		D3D12_VERTEX_BUFFER_VIEW GetView() const;
		ComPtr<ID3D12Resource> GetVertexBuffer() const;
		int VertexCount() const;

		virtual ~VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = delete;
	private:
		ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW view;
		int vertexCount;
	};

	template <typename T>
	VertexBuffer::VertexBuffer(ComPtr<ID3D12Device> device, std::vector<T> vertices) {
		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		vertexCount = vertices.size();

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = vertexCount;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		auto result = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer)
		);
		logger::CheckError(result, "Create VertexBuffer");

		{
			T* map = nullptr;
			auto result = vertexBuffer->Map(0, nullptr, (void**)&map);

			std::copy(std::begin(vertices), std::end(vertices), map);

			// 頂点を渡したので解放する
			vertexBuffer->Unmap(0, nullptr);

			// 頂点バッファビュー
			view.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			view.SizeInBytes = sizeof(vertices[0]) * vertices.size();
			view.StrideInBytes = sizeof(vertices[0]);
		}
	}
}
