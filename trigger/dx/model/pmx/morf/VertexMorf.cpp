#include "VertexMorf.h"
#include "../Header.h"
#include "../PmxVariable.h"

namespace pmx {
	VertexMorf::VertexMorf(FILE* fp, const Header& header) {
		index = PmxVariable(fp, header.VertexIndexSize()).Get();
		fread(offset, sizeof(offset), 1, fp);
	}
}
