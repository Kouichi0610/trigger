#include "MaterialMorf.h"
#include "../Header.h"
#include "../PmxVariable.h"

namespace pmx {
	MaterialMorf::MaterialMorf(FILE* fp, const Header& header) {
		index = PmxVariable(fp, header.MaterialIndexSize()).Get();
		fread(&operation, sizeof(operation), 1, fp);

		fread(diffuse, sizeof(diffuse), 1, fp);
		fread(specular, sizeof(specular), 1, fp);
		fread(&specularCoefficient, sizeof(specularCoefficient), 1, fp);
		fread(ambient, sizeof(ambient), 1, fp);
		fread(edgeColor, sizeof(edgeColor), 1, fp);
		fread(&edgeSize, sizeof(edgeSize), 1, fp);
		fread(textureCoefficient, sizeof(textureCoefficient), 1, fp);
		fread(sphereCoefficient, sizeof(sphereCoefficient), 1, fp);
		fread(toonCoefficient, sizeof(toonCoefficient), 1, fp);
	}
}
