#include "PmxRigidBody.h"
#include "Header.h"
#include "PmxName.h"
#include "PmxVariable.h"

namespace pmx {
	PmxRigidBody::PmxRigidBody(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();
		connectBoneIndex = PmxVariable(fp, header.BoneIndexSize()).Get();

		fread(&group, sizeof(group), 1, fp);
		fread(&throughCollisionGroupFlag, sizeof(throughCollisionGroupFlag), 1, fp);

		fread(&shape, sizeof(shape), 1, fp);
		fread(size, sizeof(size), 1, fp);

		fread(position, sizeof(position), 1, fp);
		fread(rotation, sizeof(rotation), 1, fp);

		fread(&mass, sizeof(mass), 1, fp);
		fread(&moveAttenuation, sizeof(moveAttenuation), 1, fp);
		fread(&rotationAttenuation, sizeof(rotationAttenuation), 1, fp);
		fread(&repulsiveForce, sizeof(repulsiveForce), 1, fp);
		fread(&friction, sizeof(friction), 1, fp);
		fread(&operation, sizeof(operation), 1, fp);
	}

	std::vector<std::unique_ptr<PmxRigidBody>> PmxRigidBody::RigidBodies(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxRigidBody>> res;
		int length;
		fread(&length, sizeof(length), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxRigidBody>(fp, header));
		}
		return res;
	}
}
