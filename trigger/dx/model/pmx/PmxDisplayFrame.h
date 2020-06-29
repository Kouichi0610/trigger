#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	class PmxDisplayFrame final {
	public:
		PmxDisplayFrame(FILE*, const Header&);

		static std::vector<std::unique_ptr<PmxDisplayFrame>> Frames(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;

		unsigned char specialFlag;	// “ÁŽê˜gƒtƒ‰ƒO

		int elementCount;
		std::vector<int> boneIndices;
		std::vector<int> morfIndices;
	};
}
