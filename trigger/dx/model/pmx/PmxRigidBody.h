#pragma once
#include <stdio.h>
#include <vector>
#include <memory>
#include <string>

namespace pmx {
	class Header;

	/// <summary>
	/// 剛体
	/// 当たり判定に関する
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
		unsigned short throughCollisionGroupFlag;	// 非衝突グループフラグ

		unsigned char shape;	// 形状 0:球 1:Box 2:カプセル
		float size[3];

		float position[3];
		float rotation[3];	// radian

		float mass;	// 質量
		float moveAttenuation;	// 移動減衰
		float rotationAttenuation;	// 回転減衰
		float repulsiveForce;	// 反発力
		float friction;	// 摩擦力

		unsigned char operation;	// 剛体の物理演算 0:ボーン追従(static) 1:物理演算(dynamic) 2:物理演算＋Bone位置合わせ
	};
}
