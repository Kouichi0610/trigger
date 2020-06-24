#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace dx {

	struct Vertex {
		XMFLOAT3 pos;
	};
	struct TextureVertex {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};


	struct TexRGBA {
		unsigned char R, G, B, A;
	};
}
