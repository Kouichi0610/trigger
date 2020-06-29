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
		// TODO:PMX�d�l�ł�bitFlag*2�ɂȂ��Ă���B���Ԃ����낤
		fread(&boneFlag, sizeof(boneFlag), 1, fp);

		// �ڑ���:1
		if ((boneFlag & 0x0001) != 0) {
			int boneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
		}
		// �ڑ���:0
		else {
			float offset[3];
			fread(offset, sizeof(offset), 1, fp);
		}
		// ��]�t�^ or �ړ��t�^1�̏ꍇ
		if (((boneFlag & 0x0100) != 0) || ((boneFlag & 0x0200) != 0)) {
			int appendParentBoneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();
			float per;	// �t�^��
			fread(&per , sizeof(per), 1, fp);
			printf("");
		}
		// ���Œ�
		if ((boneFlag & 0x0400) != 0) {
			float vector[3];
			fread(vector, sizeof(vector), 1, fp);
		}
		// ���[�J����
		if ((boneFlag & 0x0800) != 0) {
			float xvector[3];	// X���̕����x�N�g��
			float zvector[3];	// Z���̕����x�N�g��
			fread(xvector, sizeof(xvector), 1, fp);
			fread(zvector, sizeof(zvector), 1, fp);
		}
		// �O���e�ό`
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
