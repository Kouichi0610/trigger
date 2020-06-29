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

		unsigned char type;	// �W���C���g��� 0:�X�v�����O6D0F (PMX2.0�ł�0�̂�)

		// �ȉ��Atype0�̏ꍇ
		int rigidIndexA; // �֘A����A��Index �֘A�Ȃ��̏ꍇ-1
		int rigidIndexB; // �֘A����B��Index �֘A�Ȃ��̏ꍇ-1

		float position[3];
		float rotation[3];	// rad

		float moveLowerLimit[3];
		float moveUpperLimit[3];
		float rotationLowerLimit[3];
		float rotationUpperLimit[3];

		float springMoveConstant[3];	// �o�l�萔 �ړ�
		float springRotationConstant[3];	// �o�l�萔 ��]
	};
}
