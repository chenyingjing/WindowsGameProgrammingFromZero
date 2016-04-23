
//-----------------------------------������˵����----------------------------------------------
//  ��������:��D3Ddemo2
//	 2013��4�� Create by ǳī
//  ������Direct3D��Ⱦ�岽�� ʾ������
//------------------------------------------------------------------------------------------------

//-----------------------------------��ͷ�ļ��������֡�---------------------------------------
//	����������������������ͷ�ļ�
//------------------------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

#include "D3DUtil.h"
#include "DirectInputClass.h"
#include "CameraClass.h"
#include "TerrainClass.h"
#include "SkyBoxClass.h"
#include "SnowParticleClass.h"
#include "XFileModelClass.h"

//-----------------------------------�����ļ��������֡�---------------------------------------
//	���������������������Ŀ��ļ�
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")  //����PlaySound����������ļ�
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // ʹ��DirectInput���������ͷ�ļ���ע��������8
#pragma comment(lib,"dxguid.lib")

//���嶥��ṹ��
struct CUSTOMVERTEX
{
	FLOAT _x, _y, _z;
	FLOAT _nx, _ny, _nz;
	FLOAT _u, _v;
	CUSTOMVERTEX(FLOAT x, FLOAT y, FLOAT z,
		FLOAT nx, FLOAT ny, FLOAT nz, FLOAT u, FLOAT v)
	{
		_x = x, _y = y, _z = z;
		_nx = nx, _ny = ny, _nz = nz;
		_u = u, _v = v;
	}
};
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//-----------------------------------���궨�岿�֡�--------------------------------------------
//	����������һЩ������
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//Ϊ���ڿ��ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ���
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	L"������������Ϩ�����Ϸ�������롿Direct3D��Ⱦ�岽�� ʾ������"	//Ϊ���ڱ��ⶨ��ĺ�

//-----------------------------------��ȫ�ֱ����������֡�-------------------------------------
//	������ȫ�ֱ���������
//------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICE9			g_pd3dDevice = NULL; //Direct3D�豸����
ID3DXFont*					g_pTextFPS = NULL;    //����COM�ӿ�
ID3DXFont*					g_pTextAdaperName = NULL;    //����COM�ӿ�
ID3DXFont*					g_pTextHelper = NULL;    //����COM�ӿ�
ID3DXFont*					g_pTextInfo = NULL;    //����COM�ӿ�
float						g_FPS = 0.0f;       //һ�������͵ı���������֡����
wchar_t						g_strFPS[50];    //����֡���ʵ��ַ�����
wchar_t						g_strAdapterName[60] = { 0 };
XFileModelClass *g_pXFileModel1;
XFileModelClass *g_pXFileModel2;
XFileModelClass *g_pXFileModel3;
XFileModelClass *g_pXFileModel4;

DInputClass*				g_pDInput = NULL;
D3DXMATRIX					g_matWorld;

IDirect3DVertexBuffer9 *g_pVertexBuffer;
LPDIRECT3DTEXTURE9 g_pTexture;
LPD3DXMESH g_cylinder;
D3DMATERIAL9							g_MaterialCylinder; //����
CameraClass *g_pCamera;
TerrainClass *g_pTerrain;
SkyBoxClass *g_pSkyBox;
SnowParticleClass *g_pSnowParticle;


//-----------------------------------��ȫ�ֺ����������֡�-------------------------------------
//	������ȫ�ֺ�����������ֹ��δ�����ı�ʶ��ϵ�д���
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );//���ڹ��̺���
HRESULT					Direct3D_Init(HWND hwnd);		 //����������н���Direct3D�ĳ�ʼ��
HRESULT					Objects_Init(HWND hwnd); 		//����������н���Ҫ���Ƶ��������Դ��ʼ��
VOID							Direct3D_Render(HWND hwnd, FLOAT fTimeDelta); 	//����������н���Direct3D��Ⱦ�������д
VOID							Direct3D_CleanUp( );				//���������������COM��Դ�Լ�������Դ
float							Get_FPS();
void				Direct3D_Update(HWND hwnd, FLOAT fTimeDelta);
//void					Matrix_Set();
void			HelpText_Render(HWND hwnd);

//-----------------------------------��WinMain( )������--------------------------------------
//	������WindowsӦ�ó������ں��������ǵĳ�������￪ʼ
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	//��1�����ڴ����Ĳ���֮һ����ʼ���һ�������Ĵ�����
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra		= 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra		= 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon=(HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //ָ��������Ĺ������
	wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if( !RegisterClassEx( &wndClass ) )				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;		

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindow( L"ForTheDreamOfGameDevelop",WINDOW_TITLE,				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	//Direct3D��Դ�ĳ�ʼ��������ʧ����messagebox������ʾ
	if (!(S_OK==Direct3D_Init (hwnd)))
	{
		MessageBox(hwnd, _T("Direct3D��ʼ��ʧ��~��"), _T("ǳī����Ϣ����"), 0); //ʹ��MessageBox����������һ����Ϣ���� 
	}

	//��4�����ڴ����Ĳ���֮�ģ����ڵ��ƶ�����ʾ�����
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow( hwnd, nShowCmd );    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//��5����Ϣѭ������
	MSG msg = { 0 };  //��ʼ��msg
	while( msg.message != WM_QUIT )			//ʹ��whileѭ��
	{
		static FLOAT fLastTime = (float)timeGetTime();
		static FLOAT fCurrTime = (float)timeGetTime();
		static FLOAT fTimeDelta = 0.0f;

		fCurrTime = (float)timeGetTime();
		fTimeDelta = (fCurrTime - fLastTime) / 1000.0f;
		fLastTime = fCurrTime;

		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage( &msg );		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage( &msg );		//�ú����ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			Direct3D_Update(hwnd, fTimeDelta);
			Direct3D_Render(hwnd, fTimeDelta);   //������Ⱦ
		}
	}
	//��6���������ע��
	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);  //����׼��������ע��������
	return 0;  
}

//-----------------------------------��WndProc( )������--------------------------------------
//	���������ڹ��̺���WndProc,�Դ�����Ϣ���д���
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )      
{
	switch( message )						//switch��俪ʼ
	{
	case WM_PAINT:						// ���ǿͻ����ػ���Ϣ
		Direct3D_Render(hwnd, 0.0f);                 //����Direct3D��Ⱦ����
		ValidateRect(hwnd, NULL);		// ���¿ͻ�������ʾ
		break;									//������switch���

	case WM_KEYDOWN:					// ���Ǽ��̰�����Ϣ
		if (wParam == VK_ESCAPE)    // ��������µļ���ESC
			DestroyWindow(hwnd);		// ���ٴ���, ������һ��WM_DESTROY��Ϣ
		break;									//������switch���

	case WM_DESTROY:					//���Ǵ���������Ϣ
		Direct3D_CleanUp();			//�����Զ������Դ��������Game_CleanUp���������˳�ǰ����Դ����
		PostQuitMessage( 0 );			//��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;									//������switch���

	default:										//������case�����������ϣ���ִ�и�default���
		return DefWindowProc( hwnd, message, wParam, lParam );		//����ȱʡ�Ĵ��ڹ���
	}

	return 0;									//�����˳�
}

//-----------------------------------��Direct3D_Init( )������--------------------------------------
//	������Direct3D��ʼ������������Direct3D�ĳ�ʼ��
//------------------------------------------------------------------------------------------------
HRESULT Direct3D_Init(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮һ�����ӿڡ�������Direct3D�ӿڶ���, �Ա��ø�Direct3D���󴴽�Direct3D�豸����
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D�ӿڶ���Ĵ���
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //��ʼ��Direct3D�ӿڶ��󣬲�����DirectX�汾Э��
		return E_FAIL;

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮��,ȡ��Ϣ������ȡӲ���豸��Ϣ
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
	{
		return E_FAIL;
	}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //֧��Ӳ���������㣬���ǾͲ���Ӳ���������㣬���׵�
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //��֧��Ӳ���������㣬����ֻ�ò���������������

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮���������ݡ������D3DPRESENT_PARAMETERS�ṹ��
	//--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = WINDOW_WIDTH;
	d3dpp.BackBufferHeight           = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮�ģ����豸��������Direct3D�豸�ӿ�
	//--------------------------------------------------------------------------------------
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	wchar_t TempName[60] = L"��ǰ�ͺţ�";
	D3DADAPTER_IDENTIFIER9 Adapter;
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);
	int len = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_strAdapterName, len);
	wcscat_s(TempName, g_strAdapterName);
	wcscpy_s(g_strAdapterName, TempName);

	SAFE_RELEASE(pD3D); //LPDIRECT3D9�ӿڶ����ʹ����ɣ����ǽ����ͷŵ�

	if(!(S_OK==Objects_Init(hwnd))) return E_FAIL;     //����һ��Objects_Init��������Ⱦ��Դ�ĳ�ʼ��

	return S_OK;
}



//-----------------------------------��Object_Init( )������--------------------------------------
//	��������Ⱦ��Դ��ʼ���������ڴ˺����н���Ҫ����Ⱦ���������Դ�ĳ�ʼ��
//--------------------------------------------------------------------------------------------------
HRESULT Objects_Init(HWND hwnd)
{
	//��������
	if(FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 20, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("��������"), &g_pTextAdaperName)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pTextHelper)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("����"), &g_pTextInfo)))
		return E_FAIL;


	D3DXCreateCylinder(g_pd3dDevice, 280.0f, 10.0f, 3000.0f, 60, 60, &g_cylinder, 0);

	g_MaterialCylinder.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	g_MaterialCylinder.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	g_MaterialCylinder.Specular = D3DXCOLOR(0.5f, 0.0f, 0.3f, 0.3f);
	g_MaterialCylinder.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);


	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	g_pCamera = new CameraClass(g_pd3dDevice);

	g_pCamera->SetCameraPosition(&D3DXVECTOR3(0.0f, 1200.0f, -2100.0f));
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(0.0f, 1700.0f, 0.0f));

	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();

	g_pTerrain = new TerrainClass(g_pd3dDevice);
	g_pTerrain->LoadTerrainFromFile(L"GameMedia\\heighmap.raw", L"GameMedia\\terrainstone.jpg");
	g_pTerrain->InitTerrain(200, 200, 60.0f, 8.0f);

	g_pSkyBox = new SkyBoxClass(g_pd3dDevice);
	g_pSkyBox->LoadSkyTextureFromFile(L"GameMedia\\SunSetFront2048.png", L"GameMedia\\SunSetBack2048.png",
		L"GameMedia\\SunSetRight2048.png", L"GameMedia\\SunSetLeft2048.png", L"GameMedia\\SunSetUp2048.png");
	g_pSkyBox->InitSkyBox(50000);

	g_pSnowParticle = new SnowParticleClass(g_pd3dDevice);
	g_pSnowParticle->InitSnowParticle();

	HRESULT hr;
	g_pXFileModel1 = new XFileModelClass(g_pd3dDevice);
	HR(g_pXFileModel1->LoadModelFromXFile(L"knight.X"));

	g_pXFileModel2 = new XFileModelClass(g_pd3dDevice);
	HR(g_pXFileModel2->LoadModelFromXFile(L"dragon.X"));

	g_pXFileModel3 = new XFileModelClass(g_pd3dDevice);
	HR(g_pXFileModel3->LoadModelFromXFile(L"Demon.X"));

	g_pXFileModel4 = new XFileModelClass(g_pd3dDevice);
	HR(g_pXFileModel4->LoadModelFromXFile(L"angle.X"));

	return S_OK;
}


//-----------------------------------��Direct3D_Render( )������-------------------------------
//	������ʹ��Direct3D������Ⱦ
//--------------------------------------------------------------------------------------------------
void Direct3D_Render(HWND hwnd, FLOAT fTimeDelta)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 255, 255), 1.0f, 0);


	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʼ����
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->BeginScene();                     // ��ʼ����
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʽ����
	//--------------------------------------------------------------------------------------

	D3DXMATRIX mScal1, mScal2;
	D3DXMATRIX mTrans1, mTrans2, mTrans3, mTrans4;
	D3DXMATRIX mFinal1, mFinal2, mFinal3, mFinal4 ;

	D3DXMatrixTranslation(&mTrans1, 0.0f, 1200.0f, 0.0f);
	D3DXMatrixScaling(&mScal1, 3.0f, 3.0f, 3.0f);
	mFinal1 = g_matWorld * mScal1 *  mTrans1;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mFinal1);
	g_pXFileModel1->RenderModel();

	D3DXMatrixTranslation(&mTrans2, 600.0f, 0.0f, 0.0f);
	mFinal2 = mFinal1 * mTrans2;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mFinal2);
	g_pXFileModel2->RenderModel();

	D3DXMatrixTranslation(&mTrans3, -600.0f, 0.0f, 0.0f);
	mFinal3 = mFinal1 * mTrans3;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mFinal3);
	g_pXFileModel3->RenderModel();

	float s = 10.0f;
	D3DXMatrixScaling(&mScal2, s, s, s);
	D3DXMatrixTranslation(&mTrans4, 0.0f, 1500.0f, 1500.0f);
	mFinal4 = g_matWorld * mScal2 * mTrans4;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mFinal4);
	g_pXFileModel4->RenderModel();


	g_pTerrain->RenderTerrain(&g_matWorld, false);

	D3DXMATRIX matSky, matTransSky, matRotSky;
	D3DXMatrixTranslation(&matTransSky, 0.0f, -12000.0f, 0.0f);
	D3DXMatrixRotationY(&matRotSky, -0.000005f * timeGetTime());
	matSky = matTransSky * matRotSky;
	g_pSkyBox->RenderSkyBox(&matSky, false);

	g_pSnowParticle->UpdateSnowParticle(fTimeDelta);
	g_pSnowParticle->RenderSnowParticle();

	HelpText_Render(hwnd);


	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�ġ�����������
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->EndScene();                       // ��������
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�塿����ʾ��ת
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
}

//-----------------------------------��Get_FPS( )������------------------------------------------
//	���������ڼ���ÿ��֡���ʵ�һ������
//--------------------------------------------------------------------------------------------------
float Get_FPS()
{
	//�����ĸ���̬����
	static float  fps = 0; //������Ҫ�����FPSֵ
	static int     frameCount = 0;//֡��
	static float  currentTime =0.0f;//��ǰʱ��
	static float  lastTime = 0.0f;//����ʱ��

	frameCount++;//ÿ����һ��Get_FPS()������֡������1
	currentTime = timeGetTime()*0.001f;//��ȡϵͳʱ�䣬����timeGetTime�������ص����Ժ���Ϊ��λ��ϵͳʱ�䣬������Ҫ����0.001���õ���λΪ���ʱ��

	//�����ǰʱ���ȥ����ʱ�������1���ӣ��ͽ���һ��FPS�ļ���ͳ���ʱ��ĸ��£�����֡��ֵ����
	if(currentTime - lastTime > 1.0f) //��ʱ�������1����
	{
		fps = (float)frameCount /(currentTime - lastTime);//������1���ӵ�FPSֵ
		lastTime = currentTime; //����ǰʱ��currentTime��������ʱ��lastTime����Ϊ��һ��Ļ�׼ʱ��
		frameCount    = 0;//������֡��frameCountֵ����
	}

	return fps;
}

//-----------------------------------��Direct3D_CleanUp( )������--------------------------------
//	��������Դ�����������ڴ˺����н��г����˳�ǰ��Դ����������
//---------------------------------------------------------------------------------------------------
void Direct3D_CleanUp()
{
	//�ͷ�COM�ӿڶ���
	SAFE_DELETE(g_pXFileModel1);
	SAFE_DELETE(g_pXFileModel2);
	SAFE_DELETE(g_pXFileModel3);
	SAFE_DELETE(g_pXFileModel4);

	SAFE_DELETE(g_pSnowParticle);
	SAFE_DELETE(g_pDInput);
	SAFE_RELEASE(g_cylinder);
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pTextAdaperName)
	SAFE_RELEASE(g_pTextHelper)
	SAFE_RELEASE(g_pTextInfo)
	SAFE_RELEASE(g_pTextFPS)
	SAFE_RELEASE(g_pd3dDevice)
}

void Direct3D_Update(HWND hwnd, FLOAT fTimeDelta)
{
	g_pDInput->GetInput();

	if (g_pDInput->IsKeyDown(DIK_1))
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
	if (g_pDInput->IsKeyDown(DIK_2))
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	}
	if (g_pDInput->IsKeyDown(DIK_3))
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	if (g_pDInput->IsKeyDown(DIK_4))
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	}

	float k = 3.0f;
	if (g_pDInput->IsKeyDown(DIK_A))
	{
		g_pCamera->MoveAlongRightVec(-1.0f * k);
	}
	if (g_pDInput->IsKeyDown(DIK_D))
	{
		g_pCamera->MoveAlongRightVec(1.0f * k);
	}
	if (g_pDInput->IsKeyDown(DIK_W))
	{
		g_pCamera->MoveAlongLookVec(1.0f * k);
	}
	if (g_pDInput->IsKeyDown(DIK_S))
	{
		g_pCamera->MoveAlongLookVec(-1.0f * k);
	}
	if (g_pDInput->IsKeyDown(DIK_R))
	{
		g_pCamera->MoveAlongUpVec(1.0f * k);
	}
	if (g_pDInput->IsKeyDown(DIK_F))
	{
		g_pCamera->MoveAlongUpVec(-1.0f * k);
	}

	if (g_pDInput->IsKeyDown(DIK_LEFT))  g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))  g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))  g_pCamera->RotationRightVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_Q)) g_pCamera->RotationLookVec(0.001f);
	if (g_pDInput->IsKeyDown(DIK_E)) g_pCamera->RotationLookVec(-0.001f);

	//������������������������ת
	g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.001f);
	g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.001f);

	//�����ֿ��ƹ۲����������
	static FLOAT fPosZ = 0.0f;
	fPosZ += g_pDInput->MouseDZ()*0.03f;

	//���㲢����ȡ���任����
	D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	//����ȷ������任����浽g_matWorld��
	D3DXMatrixTranslation(&g_matWorld, 0.0f, 0.0f, fPosZ);


}

void HelpText_Render(HWND hwnd)
{
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//�ڴ������ϽǴ�����ʾÿ��֡��
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS());
	g_pTextFPS->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect, DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));


	formatRect.top = 30;
	static wchar_t strInfo[256] = { 0 };
	swprintf_s(strInfo, -1, L"ģ�����꣺(%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	D3DXVECTOR3 targetPosition;
	g_pCamera->GetTargetPosition(&targetPosition);
	formatRect.top += 25;
	swprintf_s(strInfo, -1, L"targetPosition��(%.2f, %.2f, %.2f)", targetPosition.x, targetPosition.y, targetPosition.z);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	D3DXVECTOR3 cameraPosition;
	g_pCamera->GetCameraPosition(&cameraPosition);
	formatRect.top += 25;
	swprintf_s(strInfo, -1, L"cameraPosition��(%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));


	formatRect.left = 0, formatRect.top = 380;
	g_pTextInfo->DrawText(NULL, L"����˵����", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(235, 123, 230, 255));

	formatRect.top += 35;
	g_pTextHelper->DrawText(NULL, L"W:��ǰ���� S:������", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"A:������� D:���ҷ���", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"R:��ֱ���Ϸ��� F:��ֱ���·���", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"Q:������б E:������б", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"�ϡ��¡����ҷ��������תģ��", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"������:����ģ��Z�᷽���ƶ�", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"ESC�����˳�����", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

}