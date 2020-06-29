#include "Header.h"
#include <string.h>
#include "PmxName.h"
#include "VertexData.h"

namespace pmx {
	Header::Header(FILE* fp) : isPmx(false), version(0) {
		char tag[4];
		fread(tag, sizeof(tag), 1, fp);
		fread(&version, sizeof(version), 1, fp);
		isPmx = strncmp(tag, "PMX ", 4) == 0;

		// 2.0‚Í8ŒÅ’è
		unsigned char datasize;
		fread(&datasize, sizeof(datasize), 1, fp);

		fread(&encodeType, sizeof(unsigned char), 1, fp);
		fread(&appendUVCount, sizeof(unsigned char), 1, fp);
		fread(&vertexIndexSize, sizeof(unsigned char), 1, fp);
		fread(&textureIndexSize, sizeof(unsigned char), 1, fp);
		fread(&materialIndexSize, sizeof(unsigned char), 1, fp);
		fread(&boneIndexSize, sizeof(unsigned char), 1, fp);
		fread(&morfIndexSize, sizeof(unsigned char), 1, fp);
		fread(&rigidBodyIndexSize, sizeof(unsigned char), 1, fp);

	}
}
