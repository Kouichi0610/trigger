#pragma once
#include "IMorfData.h"
#include <stdio.h>

namespace pmx {
	class Header;

	class GroupMorf final : public IMorfData {
	public:
		GroupMorf(FILE*, const Header&);

		virtual ~GroupMorf() = default;
		GroupMorf(const GroupMorf&) = delete;
	private:
		int morfIndex;
		float rate;
	};
}