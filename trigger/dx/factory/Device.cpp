#include "Device.h"
#include <vector>

namespace dx::factory {
	ComPtr<ID3D12Device> Device::Get() const {
		return device;
	}
	Device::Device(std::shared_ptr<logger::ILogger> logger) {
		auto level = GetNewestLevel();
		auto result = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&device));
		logger->CheckError(result, "CreateDevice");
	}

	/// <summary>
	/// ������ŐV��FeatureLevel���擾
	/// </summary>
	/// <returns></returns>
	D3D_FEATURE_LEVEL Device::GetNewestLevel() const {
		std::vector<D3D_FEATURE_LEVEL> candidates{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		for (const auto level : candidates) {
			ComPtr<ID3D12Device> d = nullptr;

			// ���:�A�_�v�^ null�Ŏ����I�� �����̃O���t�B�b�N�{�[�h����I������ۂ�
			auto res = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&d));
			if (res == S_OK && d != nullptr) {
				d = nullptr;
				return level;
			}
			d = nullptr;
		}
		// TODO:
		return (D3D_FEATURE_LEVEL)0;
	}
}
