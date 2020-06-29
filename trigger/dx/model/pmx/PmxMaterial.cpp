#include "PmxMaterial.h"
#include "PmxName.h"
#include "Header.h"
#include "PmxVariable.h"

namespace pmx {
	PmxMaterial::PmxMaterial(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();
		fread(diffuse, sizeof(diffuse), 1, fp);
		fread(specular, sizeof(specular), 1, fp);

		fread(&specularCoefficient, sizeof(specularCoefficient), 1, fp);
		fread(ambient, sizeof(ambient), 1, fp);
		fread(&bitFlag, sizeof(bitFlag), 1, fp);

		fread(edgeColor, sizeof(edgeColor), 1, fp);
		fread(&edgeSize, sizeof(edgeSize), 1, fp);
		normalTextureIndex = PmxVariable(fp, header.TextureIndexSize()).Get();
		sphereTextureIndex = PmxVariable(fp, header.TextureIndexSize()).Get();
		fread(&sphereMode, sizeof(sphereMode), 1, fp);
		fread(&commonToonFlag, sizeof(commonToonFlag), 1, fp);

		if (commonToonFlag == 0) {
			toonTextureIndex = PmxVariable(fp, header.TextureIndexSize()).Get();
			commonToonTextureIndex = 0;
		}
		else {
			toonTextureIndex = 0;
			fread(&commonToonTextureIndex, sizeof(commonToonTextureIndex), 1, fp);
		}
		memo = PmxName(fp, header).Get();
		fread(&vertexCount, sizeof(vertexCount), 1, fp);
	}

	std::vector<std::unique_ptr<PmxMaterial>> PmxMaterial::Materials(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxMaterial>> res;
		int length;
		fread(&length, sizeof(int), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxMaterial>(fp, header));
		}
		return res;
	}
}

