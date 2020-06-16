#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;

	class Factory final {
	public:
		ComPtr<IDXGIFactory6> Get() const;
		Factory(std::shared_ptr<logger::ILogger>);

		virtual ~Factory() = default;
		Factory(const Factory&) = delete;
	private:
		ComPtr<IDXGIFactory6> factory;
	};

}
