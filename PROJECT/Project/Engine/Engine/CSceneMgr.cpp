#include "pch.h"
#include "CSceneMgr.h"

#include "CEventMgr.h"
#include "CCollisionMgr.h"
#include "CResMgr.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CScene.h"
#include "CLayer.h"


CSceneMgr::CSceneMgr()
	:
	m_pCurScene(nullptr)
	, m_drawCollider{ false } {}

CSceneMgr::~CSceneMgr()
{
	for (auto& [sceneName, pScene] : m_mapScene)
	{
		if (pScene == m_pCurScene)
		{
			m_pCurScene = nullptr;
		}
		SAFE_DELETE(pScene);
	}
	SAFE_DELETE(m_pCurScene);
}


void CSceneMgr::init() { }

void CSceneMgr::progress()
{
	if (nullptr == m_pCurScene) { return; }

	SCENE_STATE eState = m_pCurScene->GetSceneState();

	if (SCENE_STATE::PLAY == eState)
	{
		m_pCurScene->update();
		m_pCurScene->lateupdate();
	}

	m_pCurScene->finalupdate();

	// Collision Check
	if (SCENE_STATE::PLAY == eState)
	{
		CCollisionMgr::GetInst()->update();
	}
}


void CSceneMgr::ChangeScene(CScene* _pNextScene, bool keep)
{
	auto iter = m_mapScene.find(_pNextScene->GetResKey());
	if (iter != m_mapScene.end())
	{
		delete _pNextScene;
		_pNextScene = iter->second;
	}
	if (nullptr != m_pCurScene)
	{
		for(int i=0;i<MAX_LAYER;++i)
		{
			_pNextScene->GetLayer(i)->SetName(m_pCurScene->GetLayer(i)->GetName());
		}
		CCollisionMgr::GetInst()->ClearCollisionInfo();

		for (auto& pTarget : m_vecDontDestroy)
		{
			CLayer* pLayer = m_pCurScene->GetLayer(pTarget->GetLayerIndex());
			auto& vecRootObjects = pLayer->GetRootObjects();

			auto iter = std::find_if(vecRootObjects.begin()
				, vecRootObjects.end()
				, [pTarget](const CGameObject* pGameObject)
				{
					return pGameObject == pTarget;
				});
			assert(iter != vecRootObjects.end());
			vecRootObjects.erase(iter);
		}

		if (m_mapScene.count(m_pCurScene->GetResKey()) == 0)
		{
			delete m_pCurScene;
		}
	}

	m_pCurScene = _pNextScene;
	if (true == keep && m_mapScene.count(m_pCurScene->GetResKey()) == 0)
	{
		m_mapScene.insert({ m_pCurScene->GetResKey(),m_pCurScene });
		m_pCurScene->SetSceneState(SCENE_STATE::PLAY);
	}
	else if(true == keep && m_mapScene.count(m_pCurScene->GetResKey()) !=0)
	{
		m_pCurScene->SetAddDonDestroy(false);
		m_pCurScene->SetSceneState(SCENE_STATE::RESUME);
	}
}

void CSceneMgr::GoToNextScene(CScene* pNextScene, bool keep)
{
	tEventInfo info{};
	info.eType = EVENT_TYPE::SCENE_CHANGE;
	info.lParam = (DWORD_PTR)pNextScene;
	info.wParam = (DWORD_PTR)keep;

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::SpawnObject(CGameObject* _pSpawnObject, Vec3 _vWorldPos, wstring _strName, UINT _iLayerIdx)
{
	tEventInfo info = {};
	info.eType = EVENT_TYPE::CREATE_OBJ;
	info.lParam = (DWORD_PTR)_pSpawnObject;
	info.wParam = static_cast<DWORD_PTR>(_iLayerIdx);
	_pSpawnObject->SetInScene(true);
	_pSpawnObject->Transform()->SetRelativePos(_vWorldPos);
	_pSpawnObject->SetName(_strName);

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::SpawnObject(CGameObject* _pSpawnObject, UINT _iLayerIdx)
{
	tEventInfo info = {};
	info.eType = EVENT_TYPE::CREATE_OBJ;
	info.lParam = (DWORD_PTR)_pSpawnObject;
	_pSpawnObject->SetInScene(true);
	info.wParam = static_cast<DWORD_PTR>(_iLayerIdx);

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::AddChild(CGameObject* _pParentObject, CGameObject* _pChildObject)
{
	tEventInfo info = {};
	info.eType = EVENT_TYPE::ADD_CHILD;
	info.lParam = (DWORD_PTR)_pParentObject;
	info.wParam = (DWORD_PTR)_pChildObject;
	_pParentObject->SetInScene(true);
	_pChildObject->SetInScene(true);

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::DisconnectParent(CGameObject* _pObject)
{
	tEventInfo info = {};
	info.eType = EVENT_TYPE::DISCONNECT_PARENT;
	info.lParam = (DWORD_PTR)_pObject;

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::ChangeObjectLayer(CGameObject* pGameObject, int layerIndex)
{
	tEventInfo info{};
	info.eType = EVENT_TYPE::CHANGE_LAYER;
	info.lParam = reinterpret_cast<DWORD_PTR>(pGameObject);
	info.wParam = static_cast<DWORD_PTR>(layerIndex);

	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::ClearLayer()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_pCurScene->GetLayer(i)->Clear();
	}
}

void CSceneMgr::DontDestroyOnLoad(CGameObject* pGameObject)
{
	m_vecDontDestroy.push_back(pGameObject);
}

void CSceneMgr::RemoveFromSceneButNotDelete(CGameObject* pGameObject)
{
	tEventInfo info{};
	info.eType = EVENT_TYPE::REMOVE_FROM_SCENE;
	info.lParam = (DWORD_PTR)pGameObject;
	pGameObject->SetInScene(false);
	CEventMgr::GetInst()->AddEvent(info);
}

CGameObject* CSceneMgr::FindObjectByName(const wstring& _strName)
{
	return GetCurScene()->FindObjectByName(_strName);
}

void CSceneMgr::FindObjectsByName(const wstring& _strName, vector<CGameObject*>& _vecOut)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = m_pCurScene->GetLayer(i);
		const vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			if (_strName == vecObj[j]->GetName())
			{
				_vecOut.push_back(vecObj[j]);
			}
		}
	}
}

std::vector<CGameObject*> CSceneMgr::GetAllObjects()
{
	std::vector<CGameObject*> vecGameObjects{};
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		auto vecObjects = m_pCurScene->GetLayer(i)->GetObjects();

		vecGameObjects.insert(vecGameObjects.end(), vecObjects.begin(), vecObjects.end());
	}

	return vecGameObjects;
}
