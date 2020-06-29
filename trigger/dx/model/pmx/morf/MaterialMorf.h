#pragma once
#include "IMorfData.h"
#include <stdio.h>

namespace pmx {
	class Header;

	class MaterialMorf final : public IMorfData {
	public:
		MaterialMorf(FILE*, const Header&);

		virtual ~MaterialMorf() = default;
		MaterialMorf(const MaterialMorf&) = delete;
	private:
		int index;	// -1�őS�ގ��Ώ�, �v�m�F
		unsigned char operation;	// �I�t�Z�b�g���Z�`�� 0:��Z 1:���Z
		float diffuse[4];
		float specular[3];
		float specularCoefficient;
		float ambient[3];
		float edgeColor[4];
		float edgeSize;
		float textureCoefficient[4];
		float sphereCoefficient[4];
		float toonCoefficient[4];
	};
}