#include "BasicShaderHeader.hlsli"
Texture2D<float4> tex : register(t0);	// 0�ԃX���b�g�ɐݒ�
SamplerState smp : register(s0);		// 0�ԃX���b�g�ɐݒ�

float4 BasicPS(Output input) : SV_TARGET{
	//return float4(1, input.uv, 1);
	//return float4(0, 0, 0, 1);
	return float4(input.normal.xyz, 1);

	// texture uv
	//return float4(tex.Sample(smp, input.uv));
}
