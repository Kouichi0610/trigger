#include "BasicShaderHeader.hlsli"

/*
	1���_���ƂɌĂ΂��
	pos ���_�f�[�^

	�R���p�C���O����
	1.�\�����[�V�����G�N�X�v���[����̃t�@�C�����E�N���b�N->�v���p�e�B
	2.�\���v���p�e�B->HLSL�R���p�C��->�S��
	�@�G���g���|�C���g���� BasicVS (���̊֐���)�ɕύX
	3.�V�F�[�_�[���f����Shader Model5.0�ȍ~�ɕύX����
*/
Output BasicVS(float4 pos : POSITION, float2 uv: TEXCOORD) {
	Output output;
	output.svpos = pos;
	output.uv = uv;
	return output;
}
