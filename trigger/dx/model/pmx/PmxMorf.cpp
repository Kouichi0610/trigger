#include "PmxMorf.h"
#include "Header.h"
#include "PmxVariable.h"
#include "PmxName.h"
#include "morf//MorfFactory.h"

namespace pmx {
	PmxMorf::PmxMorf(FILE* fp, const Header& header) {
		name = PmxName(fp, header).Get();
		enName = PmxName(fp, header).Get();

		fread(&controlPanel, sizeof(controlPanel), 1, fp);
		fread(&morfType, sizeof(morfType), 1, fp);
		//fread(&offsetCount, sizeof(offsetCount), 1, fp);
		auto factory = MorfFactory(morfType);
		morfs = factory.Create(fp, header);
	}

	std::vector<std::unique_ptr<PmxMorf>> PmxMorf::Morfs(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxMorf>> res;
		int length;
		fread(&length, sizeof(length), 1, fp);

		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxMorf>(fp, header));
		}
		return res;
	}
}