#include "BasicShaderHeader.hlsli"

// SV_TARGET レンダーターゲットに描きこむための
struct Input {
	float4 pos:POSITION;
	float4 svpos:SV_POSITION;
};

float4 BasicPS(Output input) : SV_TARGET{
	return float4(input.uv, 1, 1);
	//return float4(1, 0, 0, 0.5f);
	//return float4((float2(0,1) + input.pos.xy) * 0.5f, 1, 1);
}
