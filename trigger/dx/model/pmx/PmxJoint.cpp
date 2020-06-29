#include "PmxJoint.h"
#include "PmxVariable.h"
#include "PmxName.h"
#include "Header.h"

namespace pmx {
	PmxJoint::PmxJoint(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();

		fread(&type, sizeof(type), 1, fp);

		rigidIndexA = PmxVariable(fp, header.RigidBodyIndexSize()).Get();
		rigidIndexB = PmxVariable(fp, header.RigidBodyIndexSize()).Get();

		fread(position, sizeof(position), 1, fp);
		fread(rotation, sizeof(rotation), 1, fp);
		fread(moveLowerLimit, sizeof(moveLowerLimit), 1, fp);
		fread(moveUpperLimit, sizeof(moveUpperLimit), 1, fp);
		fread(rotationLowerLimit, sizeof(rotationLowerLimit), 1, fp);
		fread(rotationUpperLimit, sizeof(rotationUpperLimit), 1, fp);
		fread(springMoveConstant, sizeof(springMoveConstant), 1, fp);
		fread(springRotationConstant, sizeof(springRotationConstant), 1, fp);
	}

	std::vector<std::unique_ptr<PmxJoint>> PmxJoint::Joints(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxJoint>> res;
		int length;
		fread(&length, sizeof(length), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxJoint>(fp, header));
		}
		return res;
	}
}