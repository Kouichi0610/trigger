#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../../logger/ILogger.h"

namespace dx::factory {
	using Microsoft::WRL::ComPtr;
	class Device;

	/*
		コマンドアロケータはコマンドリストに格納する命令のためのメモリを管理する
		割り当てられたメモリは実行が終わるまで解放できない
		(== フレームバッファへの描画が完了する)

		フレームバッファと同じ数だけコマンドアロケータを用意するといい

	*/
	class CommandAllocator final {
	public:
		std::vector<ComPtr<ID3D12CommandAllocator>> Get() const;
		CommandAllocator(const Device&, UINT, std::shared_ptr<logger::ILogger>);

		virtual ~CommandAllocator() = default;
		CommandAllocator(const CommandAllocator&) = delete;
	private:
		std::vector<ComPtr<ID3D12CommandAllocator>> allocators;

	};

}
