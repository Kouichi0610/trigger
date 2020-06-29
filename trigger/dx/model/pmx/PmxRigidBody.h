#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	/// <summary>
	/// ฬ
	/// ฝ่ป่ษึท้
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
		unsigned short throughCollisionGroupFlag;	// ๑ีหO[vtO

		unsigned char shape;	// `๓ 0: 1:Box 2:JvZ
		float size[3];

		float position[3];
		float rotation[3];	// radian

		float mass;	// ฟส
		float moveAttenuation;	// ฺฎธ
		float rotationAttenuation;	// ๑]ธ
		float repulsiveForce;	// ฝญอ
		float friction;	// Cอ

		unsigned char operation;	// ฬฬจZ 0:{[ว](static) 1:จZ(dynamic) 2:จZ{Boneสuํน
	};
}
