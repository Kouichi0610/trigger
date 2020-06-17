#pragma once
#include <wrl.h>
#include <string>
#include <d3d12.h>
#include "../../logger/ILogger.h"

namespace dx {
	using Microsoft::WRL::ComPtr;

	struct ModelInfo {
	public:
		LPCWSTR VertexShaderFile;
		std::string VertexShaderEntry;
		LPCWSTR PixelShaderFile;
		std::string PixelShaderEntry;
		UINT64 VerticesSize;
		UINT64 IndicesSize;

		ComPtr<ID3D12Device> GetDevice() const {
			return device;
		}
		std::shared_ptr<logger::ILogger> GetLogger() const {
			return logger;
		}

		ModelInfo(ComPtr<ID3D12Device> device, std::shared_ptr<logger::ILogger> logger) {
			this->device = device;
			this->logger = logger;
			VertexShaderEntry = "";
			VertexShaderFile = L"";
			PixelShaderEntry = "";
			PixelShaderFile = L"";
			VerticesSize = 0;
			IndicesSize = 0;
		}
		virtual ~ModelInfo() = default;
	private:
		ComPtr<ID3D12Device> device;
		std::shared_ptr<logger::ILogger> logger;
	};
}