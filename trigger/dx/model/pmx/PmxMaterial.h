#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <memory>

namespace pmx {
	class Header;

	class PmxMaterial {
	public:
		PmxMaterial(FILE* fp, const Header&);
		static std::vector<std::unique_ptr<PmxMaterial>> Materials(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;
		float diffuse[4];	// rgba
		float specular[3];	// rgb
		float specularCoefficient;	// スペキュラ係数
		float ambient[3];	// rgb
		unsigned char bitFlag;
		float edgeColor[4];
		float edgeSize;

		int normalTextureIndex;
		int sphereTextureIndex;
		// スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
		unsigned char sphereMode;
		// 共有Toonフラグ 0:継続値は個別Toon 1:継続値は共有Toon
		unsigned char commonToonFlag;

		// 共有Toonフラグ0の場合のみ有効
		int toonTextureIndex;
		// 共有Toonフラグ1の場合のみ有効[0～9] それぞれ toon01.bmp～toon10.bmp に対応
		unsigned char commonToonTextureIndex;

		std::wstring memo;

		// 材質に対応する面(頂点)数 必ず3の倍数となる
		int vertexCount;
	};
	/*
		bitFlag
		0x01:両面描画, 0x02:地面影, 0x04:セルフシャドウマップへの描画, 0x08:セルフシャドウの描画,
		0x10:エッジ描画
	*/
}
