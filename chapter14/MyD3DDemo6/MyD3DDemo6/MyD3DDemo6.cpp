// MyD3DDemo6.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MyD3DDemo6.h"
#include <D3D9.h>
#include <D3dx9core.h>

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p)		if(p){p->Release();p=NULL;}

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
IDirect3DDevice9 *g_pd3dDevice;
LPD3DXFONT g_pFont = NULL;
LPD3DXMESH g_cube = NULL;
LPD3DXMESH g_teapot = NULL;
LPD3DXMESH g_Sphere = NULL;
LPD3DXMESH g_torus = NULL;


D3DXMATRIX 							g_WorldMatrix[4], R;  //定义一些全局的世界矩阵

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct3D_CleanUp();
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
    LoadStringW(hInstance, IDC_MYD3DDEMO6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
	HWND hWnd = InitInstance(hInstance, nCmdShow);
    if (!hWnd)
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYD3DDEMO6));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_MYD3DDEMO6);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

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
      CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return hWnd;
   }

   if (S_OK != Direct3D_Init(hWnd))
   {
	   MessageBox(hWnd, _T("Direct3D_Init fails."), _T("Error"), 0);
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
		Direct3D_Render(hWnd);
		ValidateRect(hWnd, NULL);
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
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
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
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
	{
		return E_FAIL;
	}
	srand(timeGetTime());
	
	if (FAILED(D3DXCreateBox(g_pd3dDevice, 2, 2, 2, &g_cube, NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTeapot(g_pd3dDevice, &g_teapot, NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSphere(g_pd3dDevice, 2, 25, 25, &g_Sphere, NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTorus(g_pd3dDevice, 0.5f, 1.5f, 25, 25, &g_torus, NULL)))
	{
		return E_FAIL;
	}



	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

void Direct3D_Render(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	Matrix_Set();

	if (GetAsyncKeyState(0x31) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	if (GetAsyncKeyState(0x32) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	D3DXMatrixRotationY(&R, timeGetTime() / 1440.0f);

	D3DXMatrixTranslation(&g_WorldMatrix[0], 3.0f, -3.0f, 0.0f);
	g_WorldMatrix[0] = g_WorldMatrix[0] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[0]);
	g_cube->DrawSubset(0);

	D3DXMatrixTranslation(&g_WorldMatrix[1], -3.0f, -3.0f, 0.0f);
	g_WorldMatrix[1] = g_WorldMatrix[1] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[1]);
	g_teapot->DrawSubset(0);

	D3DXMatrixTranslation(&g_WorldMatrix[2], 3.0f, 3.0f, 0.0f);
	g_WorldMatrix[2] = g_WorldMatrix[2] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[2]);
	g_Sphere->DrawSubset(0);

	D3DXMatrixTranslation(&g_WorldMatrix[3], -3.0f, 3.0f, 0.0f);
	g_WorldMatrix[3] = g_WorldMatrix[3] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[3]);
	g_torus->DrawSubset(0);



	TCHAR szFPS[50] = { 0 };
	swprintf_s(szFPS, _T("FPS:%.4f"), Get_FPS());
	g_pFont->DrawText(0, szFPS, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(0, 0, 0, 0);
}

void Direct3D_CleanUp()
{
	SAFE_RELEASE(g_torus);
	SAFE_RELEASE(g_Sphere);
	SAFE_RELEASE(g_teapot);
	SAFE_RELEASE(g_cube);
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

float Get_FPS()
{
	static float currentTime = 0;
	static float lastTime = 0;
	static long count = 0;
	static float fps = 0;

	count++;
	currentTime = timeGetTime() * 0.001f;
	if (currentTime - lastTime > 1)
	{
		fps = count / (currentTime - lastTime);
		count = 0;
		lastTime = currentTime;
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
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -15.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 8.0f / 6.0f, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 vp;

	vp.X = 0;
	vp.Y = 0;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&vp);
}
