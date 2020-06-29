#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <memory>

namespace pmx {
	class Header;

	class PmxMaterial {
	public:
		PmxMaterial(FILE* fp, const Header&);
		static std::vector<std::unique_ptr<PmxMaterial>> Materials(FILE*, const Header&);
	private:
		std::wstring name;
		std::wstring enName;
		float diffuse[4];	// rgba
		float specular[3];	// rgb
		float specularCoefficient;	// �X�y�L�����W��
		float ambient[3];	// rgb
		unsigned char bitFlag;
		float edgeColor[4];
		float edgeSize;

		int normalTextureIndex;
		int sphereTextureIndex;
		// �X�t�B�A���[�h 0:���� 1:��Z(sph) 2:���Z(spa) 3:�T�u�e�N�X�`��(�ǉ�UV1��x,y��UV�Q�Ƃ��Ēʏ�e�N�X�`���`����s��)
		unsigned char sphereMode;
		// ���LToon�t���O 0:�p���l�͌�Toon 1:�p���l�͋��LToon
		unsigned char commonToonFlag;

		// ���LToon�t���O0�̏ꍇ�̂ݗL��
		int toonTextureIndex;
		// ���LToon�t���O1�̏ꍇ�̂ݗL��[0�`9] ���ꂼ�� toon01.bmp�`toon10.bmp �ɑΉ�
		unsigned char commonToonTextureIndex;

		std::wstring memo;

		// �ގ��ɑΉ������(���_)�� �K��3�̔{���ƂȂ�
		int vertexCount;
	};
	/*
		bitFlag
		0x01:���ʕ`��, 0x02:�n�ʉe, 0x04:�Z���t�V���h�E�}�b�v�ւ̕`��, 0x08:�Z���t�V���h�E�̕`��,
		0x10:�G�b�W�`��
	*/
}
