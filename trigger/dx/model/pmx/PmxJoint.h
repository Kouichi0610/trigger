#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	class PmxJoint final {
	public:
		PmxJoint(FILE*, const Header&);

		static std::vector<std::unique_ptr<PmxJoint>> Joints(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;

		unsigned char type;	// ジョイント種類 0:スプリング6D0F (PMX2.0では0のみ)

		// 以下、type0の場合
		int rigidIndexA; // 関連剛体AのIndex 関連なしの場合-1
		int rigidIndexB; // 関連剛体BのIndex 関連なしの場合-1

		float position[3];
		float rotation[3];	// rad

		float moveLowerLimit[3];
		float moveUpperLimit[3];
		float rotationLowerLimit[3];
		float rotationUpperLimit[3];

		float springMoveConstant[3];	// バネ定数 移動
		float springRotationConstant[3];	// バネ定数 回転
	};
}
