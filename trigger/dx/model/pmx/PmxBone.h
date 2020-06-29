#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	class PmxBone final {
	public:
		PmxBone(FILE*, const Header&);

		static std::vector<std::unique_ptr<PmxBone>> Bones(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;
		float position[3];
		int parentBoneIndex;
		int transformHierarchy;	// •ÏŒ`ŠK‘w
		unsigned short boneFlag;

	};
}
