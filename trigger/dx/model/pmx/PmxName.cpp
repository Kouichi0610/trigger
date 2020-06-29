#include "PmxName.h"
#include "Header.h"

namespace pmx {
	std::wstring StringToWString(const std::string& arg_str) {
		// ������
		size_t length = arg_str.size();
		// �ϊ��㕶����
		size_t convLength;
		// ���̎󂯎M��p��
		wchar_t* wc = (wchar_t*)malloc(sizeof(wchar_t) * (length + 2));
		// �ϊ�
		mbstowcs_s(&convLength, wc, length + 1, arg_str.c_str(), _TRUNCATE);
		// �Ԃ�l�֕ۑ�
		std::wstring str(wc);
		// ���̎󂯎M��j��
		free(wc);
		return str;
	};

	std::vector<std::unique_ptr<PmxName>> PmxName::Names(FILE* fp, const Header& header) {
		std::vector<std::unique_ptr<PmxName>> res;
		int length;
		fread(&length, sizeof(int), 1, fp);
		for (int i = 0; i < length; i++) {
			res.emplace_back(std::make_unique<PmxName>(fp, header));
		}
		return res;
	}

	
	PmxName::PmxName(FILE* fp, const Header& header) {
		int length;
		fread(&length, sizeof(length), 1, fp);

		if (length == 0) {
			str = L"";
			return;
		}

		if (header.EncodeType() == 0) {
			wchar_t* buf = (wchar_t*)malloc(length);
			fread(buf, length, 1, fp);
			str = std::wstring(buf, length / 2);
			free(buf);
		}
		else {
			char* buf = (char*)malloc(length);
			fread(buf, length, 1, fp);
			std::string tmp(buf);
			free(buf);
			str = StringToWString(tmp);
		}
	}
}
