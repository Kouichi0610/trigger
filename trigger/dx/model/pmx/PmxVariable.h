#pragma once
#include <stdio.h>

namespace pmx {
	class PmxVariable final {
	public:
		int Get() const { return value; }
		PmxVariable(FILE*, unsigned char);
	private:
		int value;
	};
}
