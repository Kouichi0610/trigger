#include <Windows.h>

#include "dx/DXBase.h"
#include "dx/ModelFactory.h"
#include "dx/renderer/Vertex.h"
#include "dx/renderer/texture/ITexture.h"
#include "dx/renderer/texture/Texture.h"
#include "dx/renderer/texture/TextureBuffer.h"
#include "dx/renderer/Model.h"

#include "dx/shader/VertexShader.h"
#include "dx/shader/PixelShader.h"

#include "dx/model/pmd.h"
#include "dx/model/pmx.h"

#include <DirectXMath.h>

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

	auto dx = dx::DXBase();
	dx.Initialize(hwnd);

	// load texture test
	auto texture = dx.CreateTexture();
	texture->Load(L"./Resource/test.png");
	//auto textureBuffer = dx::TextureBuffer(dx.GetDevice(), L"./Resource/test.png");

	auto vs = std::make_shared <dx::VertexShader>(L"BasicVertexShader.hlsl", "BasicVS");
	auto ps = std::make_shared <dx::PixelShader>(L"BasicPixelShader.hlsl", "BasicPS");

	auto modelFactory = dx.CreateModelFactory();

#if false
	{
		auto vs = std::make_shared <dx::VertexShader>(L"SimpleVertexShader.hlsl", "BasicVS");
		auto ps = std::make_shared <dx::PixelShader>(L"SimplePixelShader.hlsl", "BasicPS");

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
#endif
	{
		std::vector<dx::TextureVertex> vertices = {
#if false
			{{-0.4f,-0.7f, 0.0f}, {0.0f, 1.0f}},
			{{-0.4f, 0.7f, 0.0f}, {0.0f, 0.0f}},
			{{ 0.4f,-0.7f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.4f, 0.7f, 0.0f}, {1.0f, 0.0f}},
#endif
#if false
			{{  0.0f,720.0f, 0.0f}, {0.0f, 1.0f}},
			{{  0.0f,  0.0f, 0.0f}, {0.0f, 0.0f}},
			{{720.0f,720.0f, 0.0f}, {1.0f, 1.0f}},
			{{720.0f,  0.0f, 0.0f}, {1.0f, 0.0f}},
#endif
			{{-1.0f,-1.0f, 0.0f}, {0.0f, 1.0f}},
			{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f,-1.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		std::vector<dx::TexRGBA> texturedata(256 * 256);
		for (auto& t : texturedata) {
			t.R = rand() % 256;
			t.G = rand() % 256;
			t.B = rand() % 256;
			t.A = 255;
		}

		auto polygon = modelFactory->Create(vertices, indices, texture.get(), vs, ps);
		//dx.Entry(polygon);
	}

	auto pmd = std::make_shared<dx::Pmd>(dx.GetDevice(), "resource/Alicia_solid.pmd", texture.get(), vs, ps);
	auto model = std::make_shared<dx::Model>(pmd);
	dx.Entry(model);

	//auto pmx = std::make_shared<dx::Pmx>(dx.GetDevice(), "resource/Alicia_solid.pmx", texture.get(), vs, ps);

#if false
	{
		// テクスチャの使いまわし可能
		std::vector<dx::TextureVertex> vertices = {
			{{-0.4f+1,-0.6f-1, 0.0f}, {0.0f, 1.0f}},
			{{-0.4f+1, 0.6f, 0.0f}, {0.0f, 0.0f}},
			{{ 0.4f+1,-0.6f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.4f+1, 0.6f+0.5f, 0.0f}, {1.0f, 0.0f}},

		};
		std::vector<unsigned short> indices = { 0,1,2, 2,1,3 };

		std::vector<dx::TexRGBA> texturedata(256 * 256);
		for (auto& t : texturedata) {
			t.R = rand() % 256;
			t.G = rand() % 256;
			t.B = rand() % 256;
			t.A = 255;
		}
		auto vs = std::make_shared <dx::VertexShader>(L"BasicVertexShader.hlsl", "BasicVS");
		auto ps = std::make_shared <dx::PixelShader>(L"BasicPixelShader.hlsl", "BasicPS");

		auto polygon = modelFactory->Create(vertices, indices, texture.get(), vs, ps);
		dx.Entry(polygon);
	}
#endif

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

