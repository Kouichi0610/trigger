#include "PmxDisplayFrame.h"
#include "PmxVariable.h"
#include "PmxName.h"
#include "Header.h"

namespace pmx {
	PmxDisplayFrame::PmxDisplayFrame(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();

		fread(&specialFlag, sizeof(specialFlag), 1, fp);
		fread(&elementCount, sizeof(elementCount), 1, fp);

		for (int i = 0; i < elementCount; i++) {
			unsigned char target;
			fread(&target, sizeof(target), 1, fp);
			// BoneIndex
			if (target == 0) {
				auto index = PmxVariable(fp, header.BoneIndexSize()).Get();
				boneIndices.emplace_back(index);
			}
			// MorfIndex
			else {
				auto index = PmxVariable(fp, header.MorfIndexSize()).Get();
				morfIndices.emplace_back(index);
			}
		}
	}

	std::vector<std::unique_ptr<PmxDisplayFrame>> PmxDisplayFrame::Frames(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxDisplayFrame>> res;
		int length;
		fread(&length, sizeof(length), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxDisplayFrame>(fp, header));
		}
		return res;
	}
}