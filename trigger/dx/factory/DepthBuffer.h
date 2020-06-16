#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;
	class HeapDsv;

	class DepthBuffer final {
	public:
		ComPtr<ID3D12Resource1> Get() const;
		DepthBuffer(const Device&, const HeapDsv&, int w, int h, std::shared_ptr<logger::ILogger>);

		virtual ~DepthBuffer() = default;
		DepthBuffer(const DepthBuffer&) = delete;
	private:
		ComPtr<ID3D12Resource1> depthBuffer;
	};
}
