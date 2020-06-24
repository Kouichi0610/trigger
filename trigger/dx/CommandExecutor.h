#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

namespace dx {
	using Microsoft::WRL::ComPtr;

	/// <summary>
	/// �R�}���h���X�g�̎��s�AGPU���̏������I������܂őҋ@
	/// 
	/// </summary>
	class CommandExecutor final {
	public:
		CommandExecutor(ComPtr<ID3D12CommandQueue>, ComPtr<ID3D12Fence>);

		void Execute(ComPtr<ID3D12CommandList>);
		void Execute(UINT, ID3D12CommandList*[]);

		virtual ~CommandExecutor() = default;
		CommandExecutor(const CommandExecutor&) = delete;
	private:
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12Fence> fence;
		int fenceValue;
	};
}
