#include "Factory.h"
#include "../../logger/Logger.h"

namespace dx::factory {
	ComPtr<IDXGIFactory6> Factory::Get() const {
		return factory;
	}
	Factory::Factory() {
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
		logger::CheckError(result, "Create Factory");
	}
}
