#include "pch.h"
#include "CSceneSaveLoad.h"

#include <Engine/CGameObject.h>
#include <Engine/CLayer.h>
#include <Engine/CPrefab.h>
#include <Engine/CScene.h>
#include <Engine/CSceneMgr.h>

#include <Engine/CScript.h>
#include "CScriptMgr.h"


void CSceneSaveLoad::SaveScene(CScene* _pScene, const wstring& _strSceneFilePath)
{
	// ���ҽ� ������� ����
	CResMgr::GetInst()->SaveChangedRes();

	// Scene ����
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strSceneFilePath.c_str(), L"wb");

	assert(pFile);
	if (nullptr == pFile) return;

	// Scene �̸� ����
	_pScene->SaveToScene(pFile);

	// Scene �� Layer ���� ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// Layer �̸� ����
		CLayer* pLayer = _pScene->GetLayer(i);
		pLayer->SaveToScene(pFile);

		// Layer �� �������� Root ������Ʈ ���� ����
		vector<CGameObject*>& vecRootObj = pLayer->GetRootObjects();
		size_t                iObjCount  = vecRootObj.size();
		fwrite(&iObjCount, sizeof(size_t), 1, pFile);

		// �� ��Ʈ������Ʈ ����
		for (const auto& pRootObject : vecRootObj)
		{
			SaveGameObject(pRootObject, pFile);
		}
	}
	fclose(pFile);
}

CScene* CSceneSaveLoad::LoadScene(const wstring& fullPath)
{
	// ���� ��ο��� ����θ� ����
	const std::wstring keyAsRelPath = CPathMgr::GetInst()->GetRelativePath(fullPath);
	// CResMgr ���� ����θ� Ű������, CSceneFile �� ã�Ƴ�
	CResMgr::GetInst()->Load<CSceneFile>(keyAsRelPath, keyAsRelPath);
	// Load �� Scene ���� �� ���Ϸκ��� �ε�
	CScene* pLoadScene = new CScene{};
	pLoadScene->SetResKey(keyAsRelPath);

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, fullPath.c_str(), L"rb");

	assert(pFile);
	if (nullptr == pFile) return nullptr;

	// Scene �̸� �ҷ�����
	pLoadScene->LoadFromScene(pFile);
	// Scene �� Layer ���� �ҷ�����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// Layer �� �̸� 
		CLayer* pLayer = pLoadScene->GetLayer(i);
		pLayer->LoadFromScene(pFile);

		// Layer ���� ������Ʈ ����
		size_t objectCount{};
		fread(&objectCount, sizeof(size_t), 1, pFile);

		// Root ������Ʈ
		for (size_t j = 0; j < objectCount; ++j)
		{
			CGameObject* pLoadObj = LoadGameObject(pFile);
			pLoadScene->AddObject(pLoadObj, i);
		}
	}
	fclose(pFile);

	return pLoadScene;
}

void CSceneSaveLoad::SaveGameObject(CGameObject* _pObj, FILE* _pFile)
{
	// �̸�, ����, ������Ʈ ����
	_pObj->SaveToScene(_pFile);

	// Script ����
	const vector<CScript*>& vecScript   = _pObj->GetScripts();
	const size_t            scriptCount = vecScript.size();

	fwrite(&scriptCount, sizeof(size_t), 1, _pFile);

	for (auto& pScript : vecScript)
	{
		std::wstring scriptName = CScriptMgr::GetScriptName(pScript);
		SaveWStringToFile(scriptName, _pFile);
		pScript->SaveToScene(_pFile);
	}
	// Child Object
	const vector<CGameObject*>& vecChild  = _pObj->GetChild();
	const size_t                childCont = vecChild.size();
	fwrite(&childCont, sizeof(size_t), 1, _pFile);
	for (auto& pChild : vecChild)
	{
		SaveGameObject(pChild, _pFile);
	}
}

CGameObject* CSceneSaveLoad::LoadGameObject(FILE* _pFile)
{
	CGameObject* pLoadObj = new CGameObject{};
	// �̸�, ����, ������Ʈ �ҷ�����
	pLoadObj->LoadFromScene(_pFile);

	// Script �ҷ�����        
	size_t scriptCount{};
	fread(&scriptCount, sizeof(size_t), 1, _pFile);
	for (size_t i = 0; i < scriptCount; ++i)
	{
		std::wstring scriptName{};
		LoadWStringFromFile(scriptName, _pFile);
		CScript* pLoadScript = CScriptMgr::GetScript(scriptName);
		pLoadObj->AddComponent(pLoadScript);
		pLoadScript->LoadFromScene(_pFile);
	}

	size_t childCount{};
	fread(&childCount, sizeof(size_t), 1, _pFile);
	for (size_t i = 0; i < childCount; ++i)
	{
		CGameObject* pChild = LoadGameObject(_pFile);
		pLoadObj->AddChild(pChild);
	}

	return pLoadObj;
}


// ======
// Prefab
// ======
void CSceneSaveLoad::SavePrefab(CPrefab* _Prefab, const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

	SaveGameObject(_Prefab->GetProto(), pFile);

	fclose(pFile);
}

int CSceneSaveLoad::LoadPrefab(CPrefab* _Prefab, const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
	if (nullptr == pFile) return E_FAIL;

	if (nullptr == _Prefab->GetProto())
	{
		CGameObject* pProto = LoadGameObject(pFile);
		_Prefab->SetProto(pProto);
	}
	fclose(pFile);

	return S_OK;
}
