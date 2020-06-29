#include "PmxVariable.h"

namespace pmx {
	PmxVariable::PmxVariable(FILE* fp, unsigned char size) : value(0) {
		if (size == 1) {
			unsigned char n;
			fread(&n, sizeof(n), 1, fp);
			value = (int)n;
		}
		else if (size == 2) {
			unsigned short n;
			fread(&n, sizeof(n), 1, fp);
			value = (int)n;
		}
		else if (size == 4) {
			fread(&value, sizeof(value), 1, fp);
		}
		else {
			throw size;
		}
	}
}
