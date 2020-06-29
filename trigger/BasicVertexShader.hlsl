#include "BasicShaderHeader.hlsli"

//�ϊ����܂Ƃ߂��\����
cbuffer cbuff0 : register(b0) {
	matrix mat;//�ϊ��s��
};

/*
	1���_���ƂɌĂ΂��
	pos ���_�f�[�^

	�R���p�C���O����
	1.�\�����[�V�����G�N�X�v���[����̃t�@�C�����E�N���b�N->�v���p�e�B
	2.�\���v���p�e�B->HLSL�R���p�C��->�S��
	�@�G���g���|�C���g���� BasicVS (���̊֐���)�ɕύX
	3.�V�F�[�_�[���f����Shader Model5.0�ȍ~�ɕύX����
*/
Output BasicVS(
		float4 pos : POSITION,
		float4 normal : NORMAL,
		float2 uv: TEXCOORD,
		min16uint2 boneno : BONE_NO,
		min16uint weight : WEIGHT
	) {
	Output output;	// �s�N�Z���V�F�[�_�֓n��
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}
