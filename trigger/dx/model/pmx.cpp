#include "pmx.h"
#include <stdio.h>
#include <DirectXMath.h>
#include "../../logger/Logger.h"
#include "../shader/VertexShader.h"
#include "../shader/PixelShader.h"
#include "../d3dx12.h"
#include "../factory/HeapSrv.h"
#include "../renderer/texture/ITexture.h"
#include <string>

#include "pmx/PmxVariable.h"
#include "pmx/Header.h"
#include "pmx/PmxName.h"
#include "pmx/PmxMaterial.h"
#include "pmx/PmxBone.h"
#include "pmx/PmxMorf.h"
#include "pmx/PmxDisplayFrame.h"
#include "pmx/PmxRigidBody.h"
#include "pmx/PmxJoint.h"

#include "pmx/VertexData.h"

namespace dx {
	using namespace DirectX;


	Pmx::Pmx(ComPtr<ID3D12Device>, const char* path, ITexture*, std::shared_ptr<VertexShader>, std::shared_ptr<PixelShader>) {
		FILE* fp;
		fopen_s(&fp, path, "rb");

		Header header(fp);


		auto modelName = PmxName(fp, header);
		auto modelEnName = PmxName(fp, header);
		auto comment = PmxName(fp, header);
		auto commentEn = PmxName(fp, header);

		int vertCount;
		fread(&vertCount, sizeof(vertCount), 1, fp);

		auto appendUV = header.AppendUVCount();
		auto boneIndexSize = header.BoneIndexSize();

		for (int i = 0; i < vertCount; i++) {
			vertices.emplace_back(VertexData(fp, appendUV, boneIndexSize));
		}

		// TODO:IndexBufferçÏÇÈÇ»ÇÁå^ä÷åWÇ»Ç¢ÇÕÇ∏
		int indexCount;
		fread(&indexCount, sizeof(indexCount), 1, fp);
		auto indexSize = indexCount * header.VertexIndexSize();
		std::vector<unsigned char> indices(indexSize);
		fread(indices.data(), sizeof(unsigned char), indexSize, fp);


		auto textures = PmxName::Names(fp, header);
		auto materials = PmxMaterial::Materials(fp, header);
		auto bones = PmxBone::Bones(fp, header);
		auto morfs =  PmxMorf::Morfs(fp, header);

		// ï\é¶òg(É{Å[Éì/ÉÇÅ[ÉtÇã§í âª)
		auto frames = PmxDisplayFrame::Frames(fp, header);

		// çÑëÃ
		auto rigidBodies = PmxRigidBody::RigidBodies(fp, header);

		auto joints = PmxJoint::Joints(fp, header);

		fclose(fp);
	}

	void Pmx::Render(ComPtr<ID3D12GraphicsCommandList>, const XMMATRIX&) {

	}
}

