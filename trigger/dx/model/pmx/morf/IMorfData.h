#pragma once

namespace pmx {
	class IMorfData {
	public:
		IMorfData() = default;
		virtual ~IMorfData() = default;
		IMorfData(const IMorfData&) = delete;
	};
}