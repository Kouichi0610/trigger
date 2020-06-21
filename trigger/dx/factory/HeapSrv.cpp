#include "HeapSrv.h"
#include "Device.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<ID3D12DescriptorHeap> HeapSrv::Get() const {
		return heapSrv;
	}
	HeapSrv::HeapSrv(ComPtr<ID3D12Device> device) {
		auto desc = D3D12_DESCRIPTOR_HEAP_DESC{};
		// ConstantBufferView, ShaderResourceView, UnorderedAccessView
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NodeMask = 0;	// GPU����������ꍇ�A���ʂ��邽�߂�
		desc.NumDescriptors = 1;
		// �V�F�[�_���猩����悤��
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		auto result = device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapSrv));
		logger::CheckError(result, "CreateSRV");
	}
}