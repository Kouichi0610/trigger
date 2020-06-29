#include "MorfFactory.h"
#include "IMorfData.h"
#include "BoneMorf.h"
#include "GroupMorf.h"
#include "MaterialMorf.h"
#include "VertexMorf.h"
#include "UVMorf.h"

namespace pmx {
	MorfFactory::MorfFactory(unsigned char morfType) : type(morfType) {
	}
	std::vector<std::unique_ptr<IMorfData>> MorfFactory::Create(FILE* fp, const Header& header) const {
		int length;
		fread(&length, sizeof(length), 1, fp);

		std::vector<std::unique_ptr<IMorfData>> res;
		for (int i = 0; i < length; i++) {
			switch (type) {
			case 0:	// グループ
				res.emplace_back(std::make_unique<GroupMorf>(fp, header));
				break;
			case 1:	// 頂点
				res.emplace_back(std::make_unique<VertexMorf>(fp, header));
				break;
			case 2:	// ボーン
				res.emplace_back(std::make_unique<BoneMorf>(fp, header));
				break;
			case 3:	// UV
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 4:	// 追加UV1 TODO:追加箇所確認
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 5:	// 追加UV2
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 6:	// 追加UV3
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 7:	// 追加UV4
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 8:	// 材質
				res.emplace_back(std::make_unique<MaterialMorf>(fp, header));
				break;
			}
		}
		return res;
	}
}