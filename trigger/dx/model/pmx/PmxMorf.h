#pragma once
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include "morf/IMorfData.h"

namespace pmx {
	class Header;
	class PmxMorf {
	public:
		PmxMorf(FILE* fp, const Header& header);

		static std::vector<std::unique_ptr<PmxMorf>> Morfs(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;
		unsigned char controlPanel;
		unsigned char morfType;
		//int offsetCount;
		std::vector<std::unique_ptr<IMorfData>> morfs;
	};
}
