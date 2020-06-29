#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include "IWeight.h"

namespace pmx {
	struct Float4 {
		float x;
		float y;
		float z;
		float w;
	};

	class VertexData {
	public:
		VertexData(FILE* fp, unsigned char appendUVCount, unsigned char boneIndexSize);

		VertexData() = default;
	private:
		// 頂点データ
		float position[3];
		float normal[3];
		float uv[2];
		std::vector<Float4> appendUV;

		std::unique_ptr<IWeight> weight;

		float edgeMag;
	};
}
