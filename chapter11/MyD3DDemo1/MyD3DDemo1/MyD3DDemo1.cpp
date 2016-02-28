// MyD3DDemo1.cpp : 定义应用程序的入口点。
//


#include "stdafx.h"
#include "MyD3DDemo1.h"

#include <D3D9.h>
#include <D3dx9core.h>
#include <fstream>

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

IDirect3DDevice9* g_pd3dDevice;
LPD3DXFONT g_pFont = NULL;

std::ofstream g_log("log\\log.txt");

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT					Direct3D_Init(HWND hwnd);		 //在这个函数中进行Direct3D的初始化
HRESULT					Objects_Init(HWND hwnd); 		//在这个函数中进行要绘制的物体的资源初始化
VOID							Direct3D_Render(HWND hwnd); 	//在这个函数中进行Direct3D渲染代码的书写
VOID							Direct3D_CleanUp();				//在这个函数中清理COM资源以及其他资源
float Get_FPS();

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
    LoadStringW(hInstance, IDC_MYD3DDEMO1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
	HWND hWnd = InitInstance(hInstance, nCmdShow);
    if (!hWnd)
    {
        return FALSE;
    }

	PlaySound(L"music\\Final Fantasy XIII.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	MSG msg = { 0 };

    // 主消息循环: 
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_MYD3DDEMO1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MYICON1));

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

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   250, 80, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return hWnd;
   }

   //Direct3D资源的初始化，成功或者失败都用messagebox予以显示
   if (S_OK != Direct3D_Init(hWnd))
   {
	   MessageBox(hWnd, L"Direct3D初始化失败~！", L"浅墨的消息窗口", 0); //使用MessageBox函数，创建一个消息窗口 
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
    //case WM_COMMAND:
    //    {
    //        int wmId = LOWORD(wParam);
    //        // 分析菜单选择: 
    //        switch (wmId)
    //        {
    //        case IDM_EXIT:
    //            DestroyWindow(hWnd);
    //            break;
    //        default:
    //            return DefWindowProc(hWnd, message, wParam, lParam);
    //        }
    //    }
    //    break;
    case WM_PAINT:
        {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            //// TODO: 在此处添加使用 hdc 的任何绘图代码...
            //EndPaint(hWnd, &ps);
			Direct3D_Render(hWnd);
			ValidateRect(hWnd, NULL);
			//return 0;
        }
        break;

	case WM_KEYDOWN:					// 若是键盘按下消息
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hWnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

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
	IDirect3D9* pD3D;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	//ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//d3dpp.BackBufferWidth = WINDOW_WIDTH;
	//d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	//d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	//d3dpp.MultiSampleQuality = 0;
	//d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.hDeviceWindow = hwnd;
	//d3dpp.Windowed = true;
	//d3dpp.EnableAutoDepthStencil = true;
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	//d3dpp.Flags = 0;
	//d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	SAFE_RELEASE(pD3D)

	if (!(S_OK == Objects_Init(hwnd))) return E_FAIL;

	return S_OK;
}

HRESULT Objects_Init(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
		DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
		return E_FAIL;
	srand(timeGetTime());
	return S_OK;
}

VOID Direct3D_Render(HWND hwnd)
{
	static DWORD currentTime = 0;
	static DWORD lastTime = 0;
	currentTime = timeGetTime();
	if (currentTime - lastTime < 16)
	{
		return;
	}
	lastTime = currentTime;

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	TCHAR szFPS[50] = { 0 };
	swprintf_s(szFPS, 50, _T("FPS:%0.3f"), Get_FPS());

	g_pFont->DrawText(NULL, szFPS, -1, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	formatRect.top = 100;
	g_pFont->DrawText(NULL, _T("你好"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(68, 139, 256));

	formatRect.top = 200;
	g_pFont->DrawText(NULL, _T("你好"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 255, 255));

	formatRect.top = 300;
	g_pFont->DrawText(NULL, _T("你好"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

VOID Direct3D_CleanUp()
{
	if (g_pFont != NULL)
		g_pFont->Release();
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

}

float Get_FPS()
{
	static float fps = 0;
	static int frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;

	frameCount++;
	currentTime = timeGetTime() * 0.001f;

	g_log << "currentTime:" << currentTime  << std::endl;

	if (currentTime - lastTime > 1.0f)
	{
		fps = frameCount / (currentTime - lastTime);
		frameCount = 0;
		lastTime = currentTime;
	}

	return fps;
}
