#pragma once
#include "ILogger.h"
#include <memory>

namespace logger {

	class LoggerFactory {
	public:
		static std::shared_ptr<ILogger> Logger();
	private:
	};
}
