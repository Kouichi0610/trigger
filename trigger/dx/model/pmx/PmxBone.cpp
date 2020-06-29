#include "PmxBone.h"
#include "Header.h"
#include "PmxName.h"
#include "PmxVariable.h"

namespace pmx {
	PmxBone::PmxBone(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();
		fread(position, sizeof(position), 1, fp);
		parentBoneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
		fread(&transformHierarchy, sizeof(transformHierarchy), 1, fp);
		// TODO:PMX仕様ではbitFlag*2になっている。たぶん一つだろう
		fread(&boneFlag, sizeof(boneFlag), 1, fp);

		// 接続先:1
		if ((boneFlag & 0x0001) != 0) {
			int boneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
		}
		// 接続先:0
		else {
			float offset[3];
			fread(offset, sizeof(offset), 1, fp);
		}
		// 回転付与 or 移動付与1の場合
		if (((boneFlag & 0x0100) != 0) || ((boneFlag & 0x0200) != 0)) {
			int appendParentBoneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
			float per;	// 付与率
			fread(&per , sizeof(per), 1, fp);
			printf("");
		}
		// 軸固定
		if ((boneFlag & 0x0400) != 0) {
			float vector[3];
			fread(vector, sizeof(vector), 1, fp);
		}
		// ローカル軸
		if ((boneFlag & 0x0800) != 0) {
			float xvector[3];	// X軸の方向ベクトル
			float zvector[3];	// Z軸の方向ベクトル
			fread(xvector, sizeof(xvector), 1, fp);
			fread(zvector, sizeof(zvector), 1, fp);
		}
		// 外部親変形
		if ((boneFlag & 0x2000) != 0) {
			int key;
			fread(&key, sizeof(key), 1, fp);
		}
		// IK
		if ((boneFlag & 0x0020) != 0) {
			int ikTargetBoneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
			int ikLoopCount;
			float ikLimitRad;
			fread(&ikLoopCount, sizeof(ikLoopCount), 1, fp);
			fread(&ikLimitRad, sizeof(ikLimitRad), 1, fp);

			int ikLinkCount;
			fread(&ikLinkCount, sizeof(ikLinkCount), 1, fp);

			for (int i = 0; i < ikLinkCount; i++) {
				int linkIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
				unsigned char limit;
				fread(&limit, sizeof(limit), 1, fp);
				if (limit == 1) {
					float lowerLimits[3];
					float upperLimits[3];
					fread(lowerLimits, sizeof(lowerLimits), 1, fp);
					fread(upperLimits, sizeof(upperLimits), 1, fp);
				}
			}
		}
		printf("");
	}
	std::vector<std::unique_ptr<PmxBone>> PmxBone::Bones(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxBone>> res;
		int length;
		fread(&length, sizeof(length), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxBone>(fp, header));
		}
		return res;
	}

}
