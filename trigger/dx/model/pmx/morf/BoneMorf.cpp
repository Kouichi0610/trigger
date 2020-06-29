#include "BoneMorf.h"
#include "../Header.h"
#include "../PmxVariable.h"

namespace pmx {
	BoneMorf::BoneMorf(FILE* fp, const Header& header) {
		boneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
		fread(move, sizeof(move), 1, fp);
		fread(rotation, sizeof(rotation), 1, fp);
	}
}
