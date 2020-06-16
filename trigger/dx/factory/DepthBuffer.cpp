#include "DepthBuffer.h"
#include "Device.h"
#include "HeapDsv.h"

namespace dx::factory {
	ComPtr<ID3D12Resource1> DepthBuffer::Get() const {
		return depthBuffer;
	}

	DepthBuffer::DepthBuffer(const Device& device, const HeapDsv& heapDsv, int width, int height, std::shared_ptr<logger::ILogger> logger) {
		{
			auto desc = D3D12_RESOURCE_DESC{};
			auto prop = D3D12_HEAP_PROPERTIES{};
			auto clearValue = D3D12_CLEAR_VALUE{};

			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 0;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			desc.Alignment = 0;

			prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = 0;
			prop.VisibleNodeMask = 0;

			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			auto result = device.Get()->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&depthBuffer)
			);
			logger->CheckError(result, "Create DepthBuffer");
		}

		// デプスステンシルビュー作成
		{
			auto desc = D3D12_DEPTH_STENCIL_VIEW_DESC {};
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Format = DXGI_FORMAT_D32_FLOAT;
			desc.Texture2D.MipSlice = 0;
			desc.Flags = D3D12_DSV_FLAG_NONE;

			auto handle = heapDsv.Get()->GetCPUDescriptorHandleForHeapStart();
			device.Get()->CreateDepthStencilView(depthBuffer.Get(), &desc, handle);
		}
	}
}