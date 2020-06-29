#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "IModelData.h"
#include "pmx/VertexData.h"

namespace dx {
	using Microsoft::WRL::ComPtr;
	using namespace pmx;
	class VertexShader;
	class PixelShader;
	class ITexture;

	/// <summary>
	/// とりあえずデータ構造の把握のみ
	/// 今のところ2.0のみ対応
	/// </summary>
	class Pmx final : public IModelData {
	public:
		Pmx(ComPtr<ID3D12Device>, const char*, ITexture*, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>);

		virtual void Render(ComPtr<ID3D12GraphicsCommandList>, const XMMATRIX&);

		Pmx(const Pmx&) = delete;
		virtual ~Pmx() = default;
	private:
		std::vector<VertexData> vertices;
	};
}
