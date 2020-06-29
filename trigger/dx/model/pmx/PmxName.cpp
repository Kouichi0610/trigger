#include "PmxName.h"
#include "Header.h"

namespace pmx {
	std::wstring StringToWString(const std::string& arg_str) {
		// 文字数
		size_t length = arg_str.size();
		// 変換後文字数
		size_t convLength;
		// 仮の受け皿を用意
		wchar_t* wc = (wchar_t*)malloc(sizeof(wchar_t) * (length + 2));
		// 変換
		mbstowcs_s(&convLength, wc, length + 1, arg_str.c_str(), _TRUNCATE);
		// 返り値へ保存
		std::wstring str(wc);
		// 仮の受け皿を破棄
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
