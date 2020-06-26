#include "IndexBuffer.h"
#include "../../logger/Logger.h"

namespace dx {
	IndexBuffer::IndexBuffer(ComPtr<ID3D12Device> device, std::vector<unsigned short> indices) {
		auto prop = D3D12_HEAP_PROPERTIES{};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 0;
		prop.VisibleNodeMask = 0;

		indexCount = indices.size();

		auto desc = D3D12_RESOURCE_DESC{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = indexCount;
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
			IID_PPV_ARGS(&indexBuffer)
		);
		logger::CheckError(result, "CreateIndexBuffer");

		{
			unsigned short* map = nullptr;
			auto result = indexBuffer->Map(0, nullptr, (void**)&map);
			std::copy(std::begin(indices), std::end(indices), map);
			indexBuffer->Unmap(0, nullptr);

			view.BufferLocation = indexBuffer->GetGPUVirtualAddress();
			view.Format = DXGI_FORMAT_R16_UINT;
			view.SizeInBytes = sizeof(indices);
		}
	}
	D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetView() const {
		return view;
	}
	int IndexBuffer::IndexCount() const {
		return indexCount;
	}
	ComPtr<ID3D12Resource> IndexBuffer::GetIndexBuffer() const {
		return indexBuffer;
	}
}
