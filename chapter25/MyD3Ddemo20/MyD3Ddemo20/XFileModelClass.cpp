#include "XFileModelClass.h"


XFileModelClass::XFileModelClass(IDirect3DDevice9 * pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
}

XFileModelClass::~XFileModelClass()
{
	SAFE_DELETE_ARRAY(m_pMaterials);

	if (m_pTextures)
	{
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			SAFE_RELEASE(m_pTextures[i]);
		}
		SAFE_DELETE_ARRAY(m_pTextures);
	}

	SAFE_RELEASE(m_pMesh);
}

HRESULT XFileModelClass::LoadModelFromXFile(WCHAR * strFilename)
{
	LPD3DXBUFFER pAdjacencyBuffer = NULL;

	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX(strFilename, D3DXMESH_MANAGED,
		m_pd3dDevice, &pAdjacencyBuffer, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh)))
	{
		return E_FAIL;
	}

	D3DXMATERIAL *d3dxMaterials = (D3DXMATERIAL *)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

	for (DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		m_pMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

		m_pTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL && strlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFileA(m_pd3dDevice,
				d3dxMaterials[i].pTextureFilename, &m_pTextures[i])))
			{
				MessageBox(NULL, L"SORRY~!没有找到纹理文件!", L"XFileModelClass类读取文件错误", MB_OK);
			}
		}
	}

	m_pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_STRIPREORDER,
		(DWORD *)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);

	SAFE_RELEASE(pAdjacencyBuffer);
	SAFE_RELEASE(pD3DXMtrlBuffer);

	return S_OK;
}

HRESULT XFileModelClass::RenderModel()
{
	for (DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		m_pd3dDevice->SetMaterial(&m_pMaterials[i]);
		m_pd3dDevice->SetTexture(0, m_pTextures[i]);
		m_pMesh->DrawSubset(i);
	}

	return S_OK;
}
