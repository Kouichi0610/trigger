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
			case 0:	// �O���[�v
				res.emplace_back(std::make_unique<GroupMorf>(fp, header));
				break;
			case 1:	// ���_
				res.emplace_back(std::make_unique<VertexMorf>(fp, header));
				break;
			case 2:	// �{�[��
				res.emplace_back(std::make_unique<BoneMorf>(fp, header));
				break;
			case 3:	// UV
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 4:	// �ǉ�UV1 TODO:�ǉ��ӏ��m�F
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 5:	// �ǉ�UV2
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 6:	// �ǉ�UV3
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 7:	// �ǉ�UV4
				res.emplace_back(std::make_unique<UVMorf>(fp, header));
				break;
			case 8:	// �ގ�
				res.emplace_back(std::make_unique<MaterialMorf>(fp, header));
				break;
			}
		}
		return res;
	}
}