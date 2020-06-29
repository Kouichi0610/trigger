#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	/// <summary>
	/// ����
	/// �����蔻��Ɋւ���
	/// </summary>
	class PmxRigidBody final {
	public:
		PmxRigidBody(FILE*, const Header&);

		static std::vector<std::unique_ptr<PmxRigidBody>> RigidBodies(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;

		int connectBoneIndex;
		unsigned char group;
		unsigned short throughCollisionGroupFlag;	// ��Փ˃O���[�v�t���O

		unsigned char shape;	// �`�� 0:�� 1:Box 2:�J�v�Z��
		float size[3];

		float position[3];
		float rotation[3];	// radian

		float mass;	// ����
		float moveAttenuation;	// �ړ�����
		float rotationAttenuation;	// ��]����
		float repulsiveForce;	// ������
		float friction;	// ���C��

		unsigned char operation;	// ���̂̕������Z 0:�{�[���Ǐ](static) 1:�������Z(dynamic) 2:�������Z�{Bone�ʒu���킹
	};
}
