
// 頂点シェーダからピクセルシェーダへのやり取り
struct Output {
	float4 svpos : SV_POSITION;	// 頂点座標
	float2 uv : TEXCOORD;	// uv
};