#include "BasicShaderHeader.hlsli"

//変換をまとめた構造体
cbuffer cbuff0 : register(b0) {
	matrix mat;//変換行列
};

/*
	1頂点ごとに呼ばれる
	pos 頂点データ

	コンパイル前準備
	1.ソリューションエクスプローラ上のファイルを右クリック->プロパティ
	2.構成プロパティ->HLSLコンパイラ->全般
	　エントリポイント名を BasicVS (この関数名)に変更
	3.シェーダーモデルをShader Model5.0以降に変更する
*/
Output BasicVS(
		float4 pos : POSITION,
		float4 normal : NORMAL,
		float2 uv: TEXCOORD,
		min16uint2 boneno : BONE_NO,
		min16uint weight : WEIGHT
	) {
	Output output;	// ピクセルシェーダへ渡す
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}
