// MyD3DDemo1.cpp : ����Ӧ�ó������ڵ㡣
//


#include "stdafx.h"
#include "MyD3DDemo1.h"

#include <D3D9.h>
#include <D3dx9core.h>
#include <fstream>

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH	800							//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

IDirect3DDevice9* g_pd3dDevice;
LPD3DXFONT g_pFont = NULL;

std::ofstream g_log("log\\log.txt");

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT					Direct3D_Init(HWND hwnd);		 //����������н���Direct3D�ĳ�ʼ��
HRESULT					Objects_Init(HWND hwnd); 		//����������н���Ҫ���Ƶ��������Դ��ʼ��
VOID							Direct3D_Render(HWND hwnd); 	//����������н���Direct3D��Ⱦ�������д
VOID							Direct3D_CleanUp();				//���������������COM��Դ�Լ�������Դ
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
    LoadStringW(hInstance, IDC_MYD3DDEMO1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
	HWND hWnd = InitInstance(hInstance, nCmdShow);
    if (!hWnd)
    {
        return FALSE;
    }

	PlaySound(L"music\\Final Fantasy XIII.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

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
			Direct3D_Render(hWnd);
		}
	}

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_MYD3DDEMO1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MYICON1));

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

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   250, 80, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return hWnd;
   }

   //Direct3D��Դ�ĳ�ʼ�����ɹ�����ʧ�ܶ���messagebox������ʾ
   if (S_OK != Direct3D_Init(hWnd))
   {
	   MessageBox(hWnd, L"Direct3D��ʼ��ʧ��~��", L"ǳī����Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ���� 
   }

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
    //case WM_COMMAND:
    //    {
    //        int wmId = LOWORD(wParam);
    //        // �����˵�ѡ��: 
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
            //// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            //EndPaint(hWnd, &ps);
			Direct3D_Render(hWnd);
			ValidateRect(hWnd, NULL);
			//return 0;
        }
        break;

	case WM_KEYDOWN:					// ���Ǽ��̰�����Ϣ
		if (wParam == VK_ESCAPE)    // ��������µļ���ESC
			DestroyWindow(hWnd);		// ���ٴ���, ������һ��WM_DESTROY��Ϣ
		break;									//������switch���

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
		DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pFont)))
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
	g_pFont->DrawText(NULL, _T("���"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(68, 139, 256));

	formatRect.top = 200;
	g_pFont->DrawText(NULL, _T("���"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 255, 255));

	formatRect.top = 300;
	g_pFont->DrawText(NULL, _T("���"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));

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
