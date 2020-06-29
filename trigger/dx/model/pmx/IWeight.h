#pragma once
#include <stdio.h>
#include <memory>

namespace pmx {
	// TODO:InterfaceŽÀ‘•
	class IWeight {
	public:
		virtual void Read(FILE*) = 0;

		IWeight() = default;
		virtual ~IWeight() = default;
	};

	std::unique_ptr<IWeight> LoadWeight(FILE*, unsigned char weightType, unsigned char boneIndexSize);
}
