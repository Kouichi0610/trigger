#include "Factory.h"

namespace dx::factory {
	ComPtr<IDXGIFactory6> Factory::Get() const {
		return factory;
	}
	Factory::Factory(std::shared_ptr<logger::ILogger> logger) {
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
		logger->CheckError(result, "Create Factory");
	}
}
