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
		int index;	// -1で全材質対象, 要確認
		unsigned char operation;	// オフセット演算形式 0:乗算 1:加算
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