#pragma once
#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#include "D3DUtil.h"

class XFileModelClass
{
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPD3DXMESH m_pMesh;
	DWORD m_dwNumMaterials;
	D3DMATERIAL9 *m_pMaterials;
	LPDIRECT3DTEXTURE9 *m_pTextures;

public:
	XFileModelClass(IDirect3DDevice9 *pd3dDevice);
	~XFileModelClass();

public:
	HRESULT LoadModelFromXFile(WCHAR *strFilename);
	HRESULT RenderModel();

};

