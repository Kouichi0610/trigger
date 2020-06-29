#pragma once
#include <stdio.h>
#include <memory>
#include "VertexData.h"

namespace pmx {
	class Header final {
	public:
		Header(FILE*);
		float Version() const { return version; }
		bool IsPmx() const { return isPmx; }

		unsigned char EncodeType() const {
			return encodeType;
		}
		unsigned char AppendUVCount() const {
			return appendUVCount; 
		}
		unsigned char BoneIndexSize() const {
			return boneIndexSize;
		}
		unsigned char VertexIndexSize() const {
			return vertexIndexSize;
		}
		unsigned char TextureIndexSize() const {
			return textureIndexSize;
		}
		unsigned char MorfIndexSize() const {
			return morfIndexSize;
		}
		unsigned char MaterialIndexSize() const {
			return materialIndexSize;
		}
		unsigned char RigidBodyIndexSize() const {
			return rigidBodyIndexSize;
		}


		virtual ~Header() = default;
	private:
		bool isPmx;
		float version;

		unsigned char encodeType;
		unsigned char appendUVCount;
		unsigned char vertexIndexSize;
		unsigned char textureIndexSize;
		unsigned char materialIndexSize;
		unsigned char boneIndexSize;
		unsigned char morfIndexSize;
		unsigned char rigidBodyIndexSize;
	};
}
