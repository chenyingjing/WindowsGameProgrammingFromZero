
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：D3Ddemo2
//	 2013年4月 Create by 浅墨
//  描述：Direct3D渲染五步曲 示例程序
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

#include "D3DUtil.h"
#include "DirectInputClass.h"
#include "CameraClass.h"
#include "TerrainClass.h"
#include "SkyBoxClass.h"

//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")  //调用PlaySound函数所需库文件
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // 使用DirectInput必须包含的头文件，注意这里有8
#pragma comment(lib,"dxguid.lib")

//定义顶点结构体
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

//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"【致我们永不熄灭的游戏开发梦想】Direct3D渲染五步曲 示例程序"	//为窗口标题定义的宏

//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICE9			g_pd3dDevice = NULL; //Direct3D设备对象
ID3DXFont*					g_pTextFPS = NULL;    //字体COM接口
ID3DXFont*					g_pTextAdaperName = NULL;    //字体COM接口
ID3DXFont*					g_pTextHelper = NULL;    //字体COM接口
ID3DXFont*					g_pTextInfo = NULL;    //字体COM接口
float						g_FPS = 0.0f;       //一个浮点型的变量，代表帧速率
wchar_t						g_strFPS[50];    //包含帧速率的字符数组
wchar_t						g_strAdapterName[60] = { 0 };
DWORD						g_dwNumMtrls;
LPD3DXMESH					g_pMesh;
D3DMATERIAL9				*g_pMaterials;
LPDIRECT3DTEXTURE9			*g_pTextures;
DInputClass*				g_pDInput = NULL;
D3DXMATRIX					g_matWorld;

IDirect3DVertexBuffer9 *g_pVertexBuffer;
LPDIRECT3DTEXTURE9 g_pTexture;
LPD3DXMESH g_cylinder;
D3DMATERIAL9							g_MaterialCylinder; //材质
CameraClass *g_pCamera;
TerrainClass *g_pTerrain;
SkyBoxClass *g_pSkyBox;


//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );//窗口过程函数
HRESULT					Direct3D_Init(HWND hwnd);		 //在这个函数中进行Direct3D的初始化
HRESULT					Objects_Init(HWND hwnd); 		//在这个函数中进行要绘制的物体的资源初始化
VOID							Direct3D_Render(HWND hwnd); 	//在这个函数中进行Direct3D渲染代码的书写
VOID							Direct3D_CleanUp( );				//在这个函数中清理COM资源以及其他资源
float							Get_FPS();
void				Direct3D_Update(HWND hwnd);
//void					Matrix_Set();
void			HelpText_Render(HWND hwnd);

//-----------------------------------【WinMain( )函数】--------------------------------------
//	描述：Windows应用程序的入口函数，我们的程序从这里开始
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	//【1】窗口创建四步曲之一：开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra		= 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra		= 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon=(HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //指定窗口类的光标句柄。
	wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if( !RegisterClassEx( &wndClass ) )				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;		

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindow( L"ForTheDreamOfGameDevelop",WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	//Direct3D资源的初始化，调用失败用messagebox予以显示
	if (!(S_OK==Direct3D_Init (hwnd)))
	{
		MessageBox(hwnd, _T("Direct3D初始化失败~！"), _T("浅墨的消息窗口"), 0); //使用MessageBox函数，创建一个消息窗口 
	}

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//【5】消息循环过程
	MSG msg = { 0 };  //初始化msg
	while( msg.message != WM_QUIT )			//使用while循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );		//该函数分发一个消息给窗口程序。
		}
		else
		{
			Direct3D_Update(hwnd);
			Direct3D_Render(hwnd);   //进行渲染
		}
	}
	//【6】窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;  
}

//-----------------------------------【WndProc( )函数】--------------------------------------
//	描述：窗口过程函数WndProc,对窗口消息进行处理
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )      
{
	switch( message )						//switch语句开始
	{
	case WM_PAINT:						// 若是客户区重绘消息
		Direct3D_Render(hwnd);                 //调用Direct3D渲染函数
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;									//跳出该switch语句

	case WM_KEYDOWN:					// 若是键盘按下消息
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

	case WM_DESTROY:					//若是窗口销毁消息
		Direct3D_CleanUp();			//调用自定义的资源清理函数Game_CleanUp（）进行退出前的资源清理
		PostQuitMessage( 0 );			//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;									//跳出该switch语句

	default:										//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程
	}

	return 0;									//正常退出
}

//-----------------------------------【Direct3D_Init( )函数】--------------------------------------
//	描述：Direct3D初始化函数，进行Direct3D的初始化
//------------------------------------------------------------------------------------------------
HRESULT Direct3D_Init(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之一，创接口】：创建Direct3D接口对象, 以便用该Direct3D对象创建Direct3D设备对象
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之二,取信息】：获取硬件设备信息
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
	{
		return E_FAIL;
	}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算，妥妥的
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //不支持硬件顶点运算，无奈只好采用软件顶点运算

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之三，填内容】：填充D3DPRESENT_PARAMETERS结构体
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
	//d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之四，创设备】：创建Direct3D设备接口
	//--------------------------------------------------------------------------------------
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	wchar_t TempName[60] = L"当前型号：";
	D3DADAPTER_IDENTIFIER9 Adapter;
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);
	int len = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_strAdapterName, len);
	wcscat_s(TempName, g_strAdapterName);
	wcscpy_s(g_strAdapterName, TempName);

	SAFE_RELEASE(pD3D); //LPDIRECT3D9接口对象的使命完成，我们将其释放掉

	if(!(S_OK==Objects_Init(hwnd))) return E_FAIL;     //调用一次Objects_Init，进行渲染资源的初始化

	return S_OK;
}



//-----------------------------------【Object_Init( )函数】--------------------------------------
//	描述：渲染资源初始化函数，在此函数中进行要被渲染的物体的资源的初始化
//--------------------------------------------------------------------------------------------------
HRESULT Objects_Init(HWND hwnd)
{
	//创建字体
	if(FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 20, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("华文中宋"), &g_pTextAdaperName)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pTextHelper)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("黑体"), &g_pTextInfo)))
		return E_FAIL;

	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX(L"95.X", D3DXMESH_MANAGED, g_pd3dDevice, &pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh)))
		return E_FAIL;

	D3DXMATERIAL *pMtrls = (D3DXMATERIAL *)pMtrlBuffer->GetBufferPointer();
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];

	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;

		g_pTextures[i] = NULL;
		D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}

	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);


	//D3DXCreateCylinder(g_pd3dDevice, 10.0f, 10.0f, 500.0f, 60, 60, &g_cylinder, 0);
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
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(0.0f, 1000.0f, -1200.0f));
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(0.0f, 1200.0f, 0.0f));
	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();

	g_pTerrain = new TerrainClass(g_pd3dDevice);
	g_pTerrain->LoadTerrainFromFile(L"GameMedia\\heighmap.raw", L"GameMedia\\terrainstone.jpg");
	g_pTerrain->InitTerrain(200, 200, 30.0f, 6.0f);

	g_pSkyBox = new SkyBoxClass(g_pd3dDevice);
	g_pSkyBox->LoadSkyTextureFromFile(L"GameMedia\\frontsnow1.jpg", L"GameMedia\\backsnow1.jpg",
		L"GameMedia\\leftsnow1.jpg", L"GameMedia\\rightsnow1.jpg", L"GameMedia\\topsnow1.jpg");
	g_pSkyBox->InitSkyBox(20000);

	return S_OK;
}


//-----------------------------------【Direct3D_Render( )函数】-------------------------------
//	描述：使用Direct3D进行渲染
//--------------------------------------------------------------------------------------------------
void Direct3D_Render(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 255, 255), 1.0f, 0);


	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之二】：开始绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->BeginScene();                     // 开始绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之三】：正式绘制
	//--------------------------------------------------------------------------------------

	D3DXMATRIX mScal, mRot1, mRot2, mTrans, mFinal;

	D3DXMatrixScaling(&mScal, 3.0f, 3.0f, 3.0f);
	D3DXMatrixTranslation(&mTrans, 0, 600, 200);
	mFinal = mScal *  mTrans * g_matWorld;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mFinal);

	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		g_pd3dDevice->SetMaterial(&g_pMaterials[i]);
		g_pd3dDevice->SetTexture(0, g_pTextures[i]);
		g_pMesh->DrawSubset(i);
	}

	g_pTerrain->RenderTerrain(&g_matWorld, false);

	D3DXMATRIX TransMatrix, RotMatrix, FinalMatrix;
	D3DXMatrixRotationX(&RotMatrix, -D3DX_PI * 0.5f);
	g_pd3dDevice->SetMaterial(&g_MaterialCylinder);
	for (size_t i = 0; i < 4; i++)
	{
		D3DXMatrixTranslation(&TransMatrix, -300, 0.0f, -350.0f + (i * 500.0f));
		FinalMatrix = RotMatrix * TransMatrix;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &FinalMatrix);
		g_cylinder->DrawSubset(0);

		D3DXMatrixTranslation(&TransMatrix, 300, 0.0f, -350.0f + (i * 500.0f));
		FinalMatrix = RotMatrix * TransMatrix;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &FinalMatrix);
		g_cylinder->DrawSubset(0);
	}

	D3DXMATRIX matSky, matTransSky, matRotSky;
	D3DXMatrixTranslation(&matTransSky, 0.0f, -3500.0f, 0.0f);
	D3DXMatrixRotationY(&matRotSky, -0.000005f * timeGetTime());
	matSky = matTransSky * matRotSky;
	g_pSkyBox->RenderSkyBox(&matSky, false);

	HelpText_Render(hwnd);


	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之四】：结束绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->EndScene();                       // 结束绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之五】：显示翻转
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}

//-----------------------------------【Get_FPS( )函数】------------------------------------------
//	描述：用于计算每秒帧速率的一个函数
//--------------------------------------------------------------------------------------------------
float Get_FPS()
{
	//定义四个静态变量
	static float  fps = 0; //我们需要计算的FPS值
	static int     frameCount = 0;//帧数
	static float  currentTime =0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

	//如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
	if(currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount /(currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		frameCount    = 0;//将本次帧数frameCount值清零
	}

	return fps;
}

//-----------------------------------【Direct3D_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
void Direct3D_CleanUp()
{
	//释放COM接口对象
	for (DWORD i = 0; i<g_dwNumMtrls; i++)
		SAFE_RELEASE(g_pTextures[i]);
	SAFE_DELETE(g_pTextures);
	SAFE_DELETE(g_pMaterials);
	SAFE_DELETE(g_pDInput);
	SAFE_RELEASE(g_cylinder);
	SAFE_RELEASE(g_pMesh);
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pTextAdaperName)
	SAFE_RELEASE(g_pTextHelper)
	SAFE_RELEASE(g_pTextInfo)
	SAFE_RELEASE(g_pTextFPS)
	SAFE_RELEASE(g_pd3dDevice)
}

void Direct3D_Update(HWND hwnd)
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

	if (g_pDInput->IsKeyDown(DIK_A))
	{
		g_pCamera->MoveAlongRightVec(-0.3f);
	}
	if (g_pDInput->IsKeyDown(DIK_D))
	{
		g_pCamera->MoveAlongRightVec(0.3f);
	}
	if (g_pDInput->IsKeyDown(DIK_W))
	{
		g_pCamera->MoveAlongLookVec(0.3f);
	}
	if (g_pDInput->IsKeyDown(DIK_S))
	{
		g_pCamera->MoveAlongLookVec(-0.3f);
	}
	if (g_pDInput->IsKeyDown(DIK_R))
	{
		g_pCamera->MoveAlongUpVec(0.3f);
	}
	if (g_pDInput->IsKeyDown(DIK_F))
	{
		g_pCamera->MoveAlongUpVec(-0.3f);
	}

	if (g_pDInput->IsKeyDown(DIK_LEFT))  g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))  g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))  g_pCamera->RotationRightVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_Q)) g_pCamera->RotationLookVec(0.001f);
	if (g_pDInput->IsKeyDown(DIK_E)) g_pCamera->RotationLookVec(-0.001f);

	//鼠标控制右向量和上向量的旋转
	g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.001f);
	g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.001f);

	//鼠标滚轮控制观察点收缩操作
	static FLOAT fPosZ = 0.0f;
	fPosZ += g_pDInput->MouseDZ()*0.03f;

	//计算并设置取景变换矩阵
	D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	//把正确的世界变换矩阵存到g_matWorld中
	D3DXMatrixTranslation(&g_matWorld, 0.0f, 0.0f, fPosZ);


}

void HelpText_Render(HWND hwnd)
{
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//在窗口右上角处，显示每秒帧数
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS());
	g_pTextFPS->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect, DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));


	formatRect.top = 30;
	static wchar_t strInfo[256] = { 0 };
	swprintf_s(strInfo, -1, L"模型坐标：(%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	D3DXVECTOR3 targetPosition;
	g_pCamera->GetTargetPosition(&targetPosition);
	formatRect.top += 25;
	swprintf_s(strInfo, -1, L"targetPosition：(%.2f, %.2f, %.2f)", targetPosition.x, targetPosition.y, targetPosition.z);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	D3DXVECTOR3 cameraPosition;
	g_pCamera->GetCameraPosition(&cameraPosition);
	formatRect.top += 25;
	swprintf_s(strInfo, -1, L"cameraPosition：(%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));


	formatRect.left = 0, formatRect.top = 380;
	g_pTextInfo->DrawText(NULL, L"控制说明：", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(235, 123, 230, 255));

	formatRect.top += 35;
	g_pTextHelper->DrawText(NULL, L"W:向前飞翔 S:向后飞翔", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"A:向左飞翔 D:向右飞翔", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"R:垂直向上飞翔 F:垂直向下飞翔", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"Q:向左倾斜 E:向右倾斜", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"上、下、左、右方向键：旋转模型", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"鼠标滚轮:人物模型Z轴方向移动", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"ESC键：退出程序", -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 200, 0, 255));

}

//void Matrix_Set()
//{
//	D3DXMATRIX matView;
//	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);
//	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
//	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
//	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
//	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
//
//	D3DXMATRIX matProj;
//	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 10000.0f);
//	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
//
//	D3DVIEWPORT9 vp;
//
//	vp.X = 0;
//	vp.Y = 0;
//	vp.Width = WINDOW_WIDTH;
//	vp.Height = WINDOW_HEIGHT;
//	vp.MinZ = 0.0f;
//	vp.MaxZ = 1.0f;
//
//	g_pd3dDevice->SetViewport(&vp);
//}
