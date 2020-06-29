#pragma once
#include "IMorfData.h"
#include <stdio.h>

namespace pmx {
	class Header;

	class UVMorf final : public IMorfData {
	public:
		UVMorf(FILE*, const Header&);

		virtual ~UVMorf() = default;
		UVMorf(const UVMorf&) = delete;
	private:
		int index;
		float offset[4]; // x,y,z,w (UV�ł�z,w�͕s�v�B�f�[�^�Ƃ��Ďc���Ă���)
	};
}