#pragma once
#include <vector>
#include <memory>
#include <stdio.h>

namespace pmx {
	class Header;
	class IMorfData;

	class MorfFactory final {
	public:
		MorfFactory(unsigned char morfType);
		std::vector<std::unique_ptr<IMorfData>> Create(FILE*, const Header&) const;
	private:
		unsigned char type;
	};
}