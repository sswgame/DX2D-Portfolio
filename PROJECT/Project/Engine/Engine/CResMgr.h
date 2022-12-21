#pragma once

#include "Ptr.h"

#include "CPathMgr.h"

#include "CPrefab.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "CTexture.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CSceneFile.h"
#include "CSound.h"
//#include "CMeshData.h"


class CResMgr final
	: public CSingleton<CResMgr>
{
	SINGLE(CResMgr);
private:
	map<wstring, CRes*> m_Res[static_cast<UINT>(RES_TYPE::END)];

public:
	void init();
	void update();
	void SaveChangedRes();

private:
	void InitSound();
	void CreateEngineMesh();
	void CreateEngineTexture();
	void CreateEngineShader();
	void CreateEngineComputeShader();
	void CreateEngineMaterial();
	void MakeInputLayoutInfo();

public:
	template <typename T>
	RES_TYPE GetResType() const;

	template <typename T>
	Ptr<T> Load(const wstring& key, const wstring& relativePath, bool isEngineResource = false);

	template <typename T>
	Ptr<T> FindRes(const wstring& key);


	const map<wstring, CRes*>& GetResList(RES_TYPE _eType) { return m_Res[static_cast<UINT>(_eType)]; }

	template <typename T>
	void AddRes(const wstring& key, T* pResource, bool isEngineResource = false);

	// _flag : D3D11_BIND_FLAG
	Ptr<CTexture> CreateTexture(const wstring& _strKey
	                          , UINT           _iWidth
	                          , UINT           _iHeight
	                          , DXGI_FORMAT    _format
	                          , UINT           _flag
	                          , bool           _bEngineRes = false);

	Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _pTex2D, bool _bEngineRes = false);

private:
	void DeleteRes(const wstring& _strKey);


	friend class CEventMgr;
};

template <typename T>
RES_TYPE CResMgr::GetResType() const
{
	if (std::is_same_v<T, CPrefab>) return RES_TYPE::PREFAB;
	if (std::is_same_v<T, CMesh>) return RES_TYPE::MESH;
	if (std::is_same_v<T, CGraphicsShader>) return RES_TYPE::GRAPHICS_SHADER;
	if (std::is_same_v<T, CComputeShader>) return RES_TYPE::COMPUTE_SHADER;
	if (std::is_same_v<T, CMaterial>) return RES_TYPE::MATERIAL;
	if (std::is_same_v<T, CTexture>) return RES_TYPE::TEXTURE;
	if (std::is_same_v<T, CSceneFile>) return RES_TYPE::SCENEFILE;
	if (std::is_same_v<T, CSound>) return RES_TYPE::SOUND;

	return RES_TYPE::END;
}

template <typename T>
Ptr<T> CResMgr::Load(const wstring& key, const wstring& relativePath, bool isEngineResource)
{
	RES_TYPE eType = GetResType<T>();

	CRes* pRes = FindRes<T>(key).Get();
	if (nullptr != pRes)
		return Ptr<T>(static_cast<T*>(pRes));

	const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	const std::wstring filePath    = contentPath + relativePath;

	pRes = new T{};
	if (FAILED(pRes->Load(filePath)))
	{
		MessageBox(nullptr, L"리소스 로딩 실패", L"리소스 로딩 오류", MB_OK);
		return nullptr;
	}

	pRes->SetKey(key);
	pRes->SetRelativePath(relativePath);
	pRes->m_bEngineRes = isEngineResource;

	m_Res[static_cast<UINT>(eType)].insert(make_pair(key, pRes));

	return Ptr<T>(static_cast<T*>(pRes));
}

template <typename T>
Ptr<T> CResMgr::FindRes(const wstring& key)
{
	RES_TYPE eType = GetResType<T>();

	auto iter = m_Res[static_cast<UINT>(eType)].find(key);

	return (iter == m_Res[static_cast<UINT>(eType)].end()) ? nullptr : static_cast<T*>(iter->second);
}

template <typename T>
void CResMgr::AddRes(const wstring& key, T* pResource, bool isEngineResource)
{
	RES_TYPE eType = GetResType<T>();

	Ptr<T> pRes = FindRes<T>(key);

	assert(nullptr == pRes);

	pResource->SetKey(key);
	pResource->SetRelativePath(key);
	pResource->m_bEngineRes = isEngineResource;

	m_Res[static_cast<UINT>(eType)].insert(make_pair(key, pResource));

	if (isEngineResource)
		return;

	wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	if (FAILED(pResource->Load(contentPath + pResource->GetRelativePath())))
	{
		pResource->Save(contentPath + pResource->GetKey());
	}
}
