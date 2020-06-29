#include "VertexData.h"
#include "IWeight.h"
#include <memory>

namespace pmx {

	VertexData::VertexData(FILE* fp, unsigned char appendUVCount, unsigned char boneIndexSize)
		: position{ 0,0,0 }, normal{ 0,0,0 }, uv{ 0,0 } {
		fread(&position, sizeof(position), 1, fp);
		fread(&normal, sizeof(normal), 1, fp);
		fread(&uv, sizeof(uv), 1, fp);

		// ’Ç‰ÁUV
		if (appendUVCount > 0) {
			appendUV = std::vector<Float4>(appendUVCount);
			fread(appendUV.data(), sizeof(Float4) * appendUVCount, 1, fp);
		}

		unsigned char weightType;
		fread(&weightType, sizeof(weightType), 1, fp);

		weight = LoadWeight(fp, weightType, boneIndexSize);

		fread(&edgeMag, sizeof(edgeMag), 1, fp);
	}

}