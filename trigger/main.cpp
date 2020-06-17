#include <Windows.h>

#include "dx/DXBase.h"
#include "logger/LoggerFactory.h"

#include "dx/renderer/ModelInfo.h"
#include "dx/renderer/Polygon.h"

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウ破棄時に呼ばれる
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	w.lpszClassName = TEXT("trigger");	// アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc = { 0, 0, 1280, 720 };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(w.lpszClassName,
		TEXT("trigger"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	auto logger = logger::LoggerFactory().Logger();
	auto dx = dx::DXBase(logger);
	dx.Initialize(hwnd);

	{
		std::vector<dx::Polygon::Vertex> vertices = {
			{{-0.4f,-0.7f, 0.0f}, {0.0f, 1.0f}},
			{{-0.4f, 0.7f, 0.0f}, {0.0f, 0.0f}},
			{{ 0.4f,-0.7f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.4f, 0.7f, 0.0f}, {1.0f, 0.0f}},

		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		auto info = dx.CreateModelInfo();
		info->IndicesSize = indices.size();
		info->VerticesSize = vertices.size();
		info->VertexShaderFile = L"BasicVertexShader.hlsl";
		info->VertexShaderEntry = "BasicVS";
		info->PixelShaderFile = L"BasicPixelShader.hlsl";
		info->PixelShaderEntry = "BasicPS";

		auto polygon = std::make_shared<dx::Polygon>();
		polygon->Init(*info, vertices, indices);
		dx.Entry(polygon);
	}

	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {
			break;
		}

		dx.Main();
	}
	UnregisterClass(w.lpszClassName, w.hInstance);
	return 0;
}
