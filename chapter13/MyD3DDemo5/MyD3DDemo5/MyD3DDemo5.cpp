// MyD3DDemo5.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MyD3DDemo5.h"

#include <D3D9.h>
#include <D3dx9core.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_LOADSTRING 100
#define SAFE_RELEASE(p)		if(p){p->Release();p=NULL;}

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
IDirect3DDevice9* g_pd3dDevice;
LPD3DXFONT g_Font;
IDirect3DVertexBuffer9* g_pVertexBuffer;
IDirect3DIndexBuffer9* g_pIndexBuffer;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
VOID Direct3D_Render(HWND hwnd);
VOID Direct3D_CleanUp();
float Get_FPS();
VOID Matrix_Set();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MYD3DDEMO5, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	HWND hWnd = InitInstance(hInstance, nCmdShow);
	if (!hWnd)
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYD3DDEMO5));

	MSG msg = { 0 };

	// 主消息循环: 
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3D_Render(hWnd);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_MYD3DDEMO5);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return hWnd;
	}

	if (S_OK != Direct3D_Init(hWnd))
	{
		MessageBox(hWnd, _T("Direct3D_Init 失败"), _T("message"), 0);
		return NULL;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_PAINT:
	{
		Direct3D_Render(hWnd);
		ValidateRect(hWnd, NULL);
	}
	break;
	case WM_DESTROY:
		Direct3D_CleanUp();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HRESULT Direct3D_Init(HWND hwnd)
{
	IDirect3D9 *pD3D;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	SAFE_RELEASE(pD3D);

	if (S_OK != Objects_Init(hwnd))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Objects_Init(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_Font)))
	{
		return E_FAIL;
	}
	srand(timeGetTime());

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(12 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(42 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIndexBuffer, NULL)))
	{
		return E_FAIL;
	}

	CUSTOMVERTEX Vertices[] =
	{
		{ -20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },

		{ 50.0f, 0.0f, 50.0f, D3DCOLOR_XRGB(255, 255, 255) },//8
		{ 50.0f, 0.0f, -50.0f, D3DCOLOR_XRGB(255, 255, 255) },//9
		{ -50.0f, 0.0f, -50.0f, D3DCOLOR_XRGB(255, 255, 255) },//10
		{ -50.0f, 0.0f, 50.0f, D3DCOLOR_XRGB(255, 255, 255) }//11
	};

	VOID *pVertices;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(Vertices), &pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, Vertices, sizeof(Vertices));
	g_pVertexBuffer->Unlock();

	WORD *pIndices = NULL;
	g_pIndexBuffer->Lock(0, 0, (VOID**)&pIndices, 0);

	//pIndices[0] = 0, pIndices[1] = 1, pIndices[2] = 2;
	//pIndices[3] = 0, pIndices[4] = 2, pIndices[5] = 3;

	//pIndices[6] = 0, pIndices[7] = 3, pIndices[8] = 7;
	//pIndices[9] = 0, pIndices[10] = 7, pIndices[11] = 4;

	//pIndices[12] = 0, pIndices[13] = 4, pIndices[14] = 5;
	//pIndices[15] = 0, pIndices[16] = 5, pIndices[17] = 1;

	//pIndices[18] = 2, pIndices[19] = 6, pIndices[20] = 7;
	//pIndices[21] = 2, pIndices[22] = 7, pIndices[23] = 3;

	//pIndices[24] = 2, pIndices[25] = 5, pIndices[26] = 6;
	//pIndices[27] = 2, pIndices[28] = 1, pIndices[29] = 5;

	//pIndices[30] = 4, pIndices[31] = 6, pIndices[32] = 5;
	//pIndices[33] = 4, pIndices[34] = 7, pIndices[35] = 6;

	//pIndices[36] = 10, pIndices[37] = 11, pIndices[38] = 8;
	//pIndices[39] = 10, pIndices[40] = 8, pIndices[41] = 9;

	pIndices[0] = 10, pIndices[1] = 11, pIndices[2] = 8;
	pIndices[3] = 10, pIndices[4] = 8, pIndices[5] = 9;

	pIndices[6] = 0, pIndices[7] = 1, pIndices[8] = 2;
	pIndices[9] = 0, pIndices[10] = 2, pIndices[11] = 3;

	pIndices[12] = 0, pIndices[13] = 3, pIndices[14] = 7;
	pIndices[15] = 0, pIndices[16] = 7, pIndices[17] = 4;

	pIndices[18] = 0, pIndices[19] = 4, pIndices[20] = 5;
	pIndices[21] = 0, pIndices[22] = 5, pIndices[23] = 1;

	pIndices[24] = 2, pIndices[25] = 6, pIndices[26] = 7;
	pIndices[27] = 2, pIndices[28] = 7, pIndices[29] = 3;

	pIndices[30] = 2, pIndices[31] = 5, pIndices[32] = 6;
	pIndices[33] = 2, pIndices[34] = 1, pIndices[35] = 5;

	pIndices[36] = 4, pIndices[37] = 6, pIndices[38] = 5;
	pIndices[39] = 4, pIndices[40] = 7, pIndices[41] = 6;


	g_pIndexBuffer->Unlock();

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

VOID Direct3D_Render(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	Matrix_Set();

	if (GetAsyncKeyState(0x31) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	if (GetAsyncKeyState(0x32) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}


	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->SetIndices(g_pIndexBuffer);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 12, 0, 14);

	TCHAR szFPS[50] = { 0 };
	swprintf_s(szFPS, 50, _T("FPS:%0.3f"), Get_FPS());
	g_Font->DrawText(NULL, szFPS, -1, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(0, 0, 0, 0);
}

VOID Direct3D_CleanUp()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_Font);
	SAFE_RELEASE(g_pd3dDevice);
}

float Get_FPS()
{
	static float fps = 0;
	static float frameCount = 0;
	static float currentTime = 0;
	static float lastTime = 0;

	frameCount++;

	currentTime = timeGetTime() * 0.001f;

	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}

VOID Matrix_Set()
{
	D3DXMATRIX matWorld, Rx, Ry, Rz;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixRotationX(&Rx, D3DX_PI * (timeGetTime() / 1000.0f));
	//D3DXMatrixRotationY(&Ry, D3DX_PI * (timeGetTime() / 1000.0f / 2));
	//D3DXMatrixRotationZ(&Rz, D3DX_PI * (timeGetTime() / 1000.0f / 3));

	//matWorld = Rx * Ry * Rz * matWorld;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(30.0f, 60.0f, -200.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 8.0/6.0, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 vp;

	vp.X = 0;
	vp.Y = 0;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&vp);

}