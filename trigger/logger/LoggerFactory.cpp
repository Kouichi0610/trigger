#include "LoggerFactory.h"
#include "components/Logger.h"

namespace logger {
	std::shared_ptr<ILogger> LoggerFactory::Logger() {
		return std::make_shared<logger::Logger>();
	}
}
