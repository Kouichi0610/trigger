#include <Windows.h>

#include "dx/DXBase.h"
#include "logger/LoggerFactory.h"

#include "dx/renderer/ModelFactory.h"
#include "dx/renderer/Vertex.h"

#include "dx/shader/VertexShader.h"
#include "dx/shader/PixelShader.h"

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

	auto modelFactory = dx.CreateModelFactory();
	{
		auto vs = std::make_shared <dx::VertexShader>(L"SimpleVertexShader.hlsl", "BasicVS", logger);
		auto ps = std::make_shared <dx::PixelShader>(L"SimplePixelShader.hlsl", "BasicPS", logger);

		std::vector<dx::Vertex> vertices = {
			{{0.6f,-0.4f, 0.0f}},
			{{0.6f, 0.0f, 0.0f}},
			{{0.8f,-0.4f, 0.0f}},
			{{0.8f, 0.0f, 0.0f}},

		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };
		auto polygon = modelFactory->Create(vertices, indices, vs, ps);
		dx.Entry(polygon);
	}
	{
		std::vector<dx::TextureVertex> vertices = {
			{{-0.4f,-0.7f, 0.0f}, {0.0f, 1.0f}},
			{{-0.4f, 0.7f, 0.0f}, {0.0f, 0.0f}},
			{{ 0.4f,-0.7f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.4f, 0.7f, 0.0f}, {1.0f, 0.0f}},

		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		std::vector<dx::TexRGBA> texturedata(256 * 256);
		for (auto& t : texturedata) {
			t.R = rand() % 256;
			t.G = rand() % 256;
			t.B = rand() % 256;
			t.A = 255;
		}
		auto vs = std::make_shared <dx::VertexShader>(L"BasicVertexShader.hlsl", "BasicVS", logger);
		auto ps = std::make_shared <dx::PixelShader>(L"BasicPixelShader.hlsl", "BasicPS", logger);

		auto polygon = modelFactory->Create(vertices, indices, texturedata, 256, 256, vs, ps);
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
