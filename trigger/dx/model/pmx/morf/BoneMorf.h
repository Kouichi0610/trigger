#pragma once
#include "IMorfData.h"
#include <stdio.h>

namespace pmx {
	class Header;

	class BoneMorf final : public IMorfData {
	public:
		BoneMorf(FILE*, const Header&);

		virtual ~BoneMorf() = default;
		BoneMorf(const BoneMorf&) = delete;
	private:
		int boneIndex;
		float move[3];
		float rotation[4];	// クォータニオン(x, y, z, w)
	};
}