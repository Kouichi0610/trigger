#include "GroupMorf.h"
#include "../Header.h"
#include "../PmxVariable.h"

namespace pmx {
	GroupMorf::GroupMorf(FILE* fp, const Header& header) {
		morfIndex = PmxVariable(fp, header.MorfIndexSize()).Get();
		fread(&rate, sizeof(rate), 1, fp);
	}
}
