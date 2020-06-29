#pragma once
#include "IMorfData.h"
#include <stdio.h>

namespace pmx {
	class Header;

	class VertexMorf final : public IMorfData {
	public:
		VertexMorf(FILE*, const Header&);

		virtual ~VertexMorf() = default;
		VertexMorf(const VertexMorf&) = delete;
	private:
		int index;
		float offset[3];	// 座標オフセット量(x,y,z);
	};
}