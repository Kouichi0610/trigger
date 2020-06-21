#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	/*
		�R�}���h�A���P�[�^�̓R�}���h���X�g�Ɋi�[���閽�߂̂��߂̃��������Ǘ�����
		���蓖�Ă�ꂽ�������͎��s���I���܂ŉ���ł��Ȃ�
		(== �t���[���o�b�t�@�ւ̕`�悪��������)

		�t���[���o�b�t�@�Ɠ����������R�}���h�A���P�[�^��p�ӂ���Ƃ���

	*/
	class CommandAllocator final {
	public:
		std::vector<ComPtr<ID3D12CommandAllocator>> Get() const;
		CommandAllocator(const Device&, UINT);

		virtual ~CommandAllocator() = default;
		CommandAllocator(const CommandAllocator&) = delete;
	private:
		std::vector<ComPtr<ID3D12CommandAllocator>> allocators;

	};

}
