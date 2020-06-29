#include "IWeight.h"

namespace pmx {
	template <typename T>
	class WeightBdef1 : public IWeight {
	public:
		WeightBdef1() : bone(0) {}
		virtual void Read(FILE* fp) {
			fread(&bone, sizeof(T), 1, fp);
		}
	private:
		// ウェイト1.0の単一ボーン(参照インデックス)
		T bone;
	};
	template <typename T>
	class WeightBdef2 : public IWeight {
	public:
		WeightBdef2() : bone1(0), bone2(0), weight1(0) {}
		virtual void Read(FILE* fp) {
			fread(&bone1, sizeof(T), 1, fp);
			fread(&bone2, sizeof(T), 1, fp);
			fread(&weight1, sizeof(weight1), 1, fp);
		}
	private:
		T bone1;
		T bone2;
		float weight1;	// 0.0～1.0
		// bone2のウェイトは1-weight1
	};
	template <typename T>
	class WeightBdef4 : public IWeight {
	public:
		WeightBdef4() {}
		virtual void Read(FILE* fp) {
			fread(bones, sizeof(bones), 1, fp);
			fread(weights, sizeof(weights), 1, fp);
		}
	private:
		T bones[4];
		float weights[4];
	};

	template <typename T>
	class WeightSdef : public IWeight {
	public:
		WeightSdef() {}
		virtual void Read(FILE* fp) {
			fread(&bone1, sizeof(T), 1, fp);
			fread(&bone2, sizeof(T), 1, fp);
			fread(&weight1, sizeof(weight1), 1, fp);
			fread(sdef_c, sizeof(sdef_c), 1, fp);
			fread(sdef_r0, sizeof(sdef_r0), 1, fp);
			fread(sdef_r1, sizeof(sdef_r1), 1, fp);
		}
	private:
		T bone1;
		T bone2;
		float weight1;	// 0.0～1.0
		float sdef_c[3];
		float sdef_r0[3];
		float sdef_r1[3];
	};


	std::unique_ptr<IWeight> Create(unsigned char weightType, unsigned char boneIndexSize);
	std::unique_ptr<IWeight> CreateByte(unsigned char weightType);
	std::unique_ptr<IWeight> CreateShort(unsigned char weightType);
	std::unique_ptr<IWeight> CreateInt(unsigned char weightType);

	std::unique_ptr<IWeight> LoadWeight(FILE* fp, unsigned char weightType, unsigned char boneIndexSize) {
		auto res = Create(weightType, boneIndexSize);
		res->Read(fp);
		return res;
	}

	std::unique_ptr<IWeight> Create(unsigned char weightType, unsigned char boneIndexSize) {
		switch (boneIndexSize) {
		case 1:
			return CreateByte(weightType);
		case 2:
			return CreateShort(weightType);
		case 4:
			return CreateInt(weightType);
		default:
			throw boneIndexSize;
		}
	}


	std::unique_ptr<IWeight> CreateByte(unsigned char weightType) {
		switch (weightType) {
		case 0:
			return std::make_unique<WeightBdef1<unsigned char>>();
		case 1:
			return std::make_unique<WeightBdef2<unsigned char>>();
		case 2:
			return std::make_unique<WeightBdef4<unsigned char>>();
		case 3:
			return std::make_unique<WeightSdef<unsigned char>>();
		default:
			throw weightType;
		}
		return nullptr;
	}
	std::unique_ptr<IWeight> CreateShort(unsigned char weightType) {
		switch (weightType) {
		case 0:
			return std::make_unique<WeightBdef1<unsigned short>>();
		case 1:
			return std::make_unique<WeightBdef2<unsigned short>>();
		case 2:
			return std::make_unique<WeightBdef4<unsigned short>>();
		case 3:
			return std::make_unique<WeightSdef<unsigned short>>();
		default:
			throw weightType;
		}
		return nullptr;
	}
	std::unique_ptr<IWeight> CreateInt(unsigned char weightType) {
		switch (weightType) {
		case 0:
			return std::make_unique<WeightBdef1<int>>();
		case 1:
			return std::make_unique<WeightBdef2<int>>();
		case 2:
			return std::make_unique<WeightBdef4<int>>();
		case 3:
			return std::make_unique<WeightSdef<int>>();
		default:
			throw weightType;
		}
		return nullptr;
	}
}
