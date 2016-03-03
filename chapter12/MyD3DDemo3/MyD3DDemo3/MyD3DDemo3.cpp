// MyD3DDemo3.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MyD3DDemo3.h"

#include <D3D9.h>
#include <D3dx9core.h>

#define WINDOW_WIDTH	800							//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define MAX_LOADSTRING 100
#define SAFE_RELEASE(p)		if(p){p->Release();p=NULL;}

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX		(D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
IDirect3DDevice9 *g_pd3dDevice;
LPD3DXFONT g_pFont;
IDirect3DVertexBuffer9 *g_pVertexBuffer;


// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct3D_CleanUp();
float Get_FPS();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYD3DDEMO3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}
	HWND hwnd = InitInstance(hInstance, nCmdShow);
	if (!hwnd)
	{
		return FALSE;
	}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYD3DDEMO3));

	MSG msg = { 0 };

    // ����Ϣѭ��: 
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3D_Render(hwnd);
		}
	}


    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}

    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
	wcex.lpszMenuName	= 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return hWnd;
   }

   Direct3D_Init(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
			Direct3D_Render(hWnd);
			ValidateRect(hWnd, NULL);
        }
        break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
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
	IDirect3D9 *pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	SAFE_RELEASE(pD3D)

	if (!(S_OK == Objects_Init(hwnd)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Objects_Init(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pFont)))
	{
		return E_FAIL;
	}
	srand(timeGetTime());

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}

	CUSTOMVERTEX vertices[] =
	{
		{ 300.0f, 100.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 500.0f, 100.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 300.0f, 300.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 300.0f, 300.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		//{ (float)(800.0*rand() / (RAND_MAX + 1.0)), (float)(600.0*rand() / (RAND_MAX + 1.0)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		//{ (float)(800.0*rand() / (RAND_MAX + 1.0)), (float)(600.0*rand() / (RAND_MAX + 1.0)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) }
		{ (float)(rand() % WINDOW_WIDTH), (float)(rand() % WINDOW_HEIGHT), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ (float)(rand() % WINDOW_WIDTH), (float)(rand() % WINDOW_HEIGHT), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) }
	};

	VOID *pVertices;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVertexBuffer->Unlock();

	return S_OK;
}

void Direct3D_Render(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1, 0);
	g_pd3dDevice->BeginScene();

	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	TCHAR szFPS[50] = { 0 };
	swprintf_s(szFPS, 50, _T("FPS:%0.3f"), Get_FPS());
	g_pFont->DrawText(0, szFPS, -1, &formatRect, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 39, 136));

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(0, 0, 0, 0);
}

void Direct3D_CleanUp()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

float Get_FPS()
{
	static float fps = 0;
	static float currentTime = 0;
	static float lastTime = 0;
	static long count = 0;

	count++;
	currentTime = timeGetTime() * 0.001f;

	if (currentTime - lastTime > 1)
	{
		fps = count / (currentTime - lastTime);
		lastTime = currentTime;
		count = 0;
	}
	return fps;
}