#pragma once
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>

namespace pmx {
	class Header;
	class PmxName {
	public:
		PmxName(FILE* fp, const Header& header);

		static std::vector<std::unique_ptr<PmxName>> Names(FILE*, const Header&);

		std::wstring Get() const { return str; }
	private:
		std::wstring str;
	};
}
