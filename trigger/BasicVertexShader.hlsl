#include "BasicShaderHeader.hlsli"

/*
	1頂点ごとに呼ばれる
	pos 頂点データ

	コンパイル前準備
	1.ソリューションエクスプローラ上のファイルを右クリック->プロパティ
	2.構成プロパティ->HLSLコンパイラ->全般
	　エントリポイント名を BasicVS (この関数名)に変更
	3.シェーダーモデルをShader Model5.0以降に変更する
*/
Output BasicVS(float4 pos : POSITION, float2 uv: TEXCOORD) {
	Output output;
	output.svpos = pos;
	output.uv = uv;
	return output;
}
