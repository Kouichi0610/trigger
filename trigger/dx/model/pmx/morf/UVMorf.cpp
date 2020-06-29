#include "UVMorf.h"
#include "../Header.h"
#include "../PmxVariable.h"

namespace pmx {
	UVMorf::UVMorf(FILE* fp, const Header& header) {
		index = PmxVariable(fp, header.VertexIndexSize()).Get();
		fread(offset, sizeof(offset), 1, fp);
	}
}
