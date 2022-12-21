#include "pch.h"
#include "CScene.h"

#include <deque>

#include "CEventMgr.h"
#include "CRenderMgr.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CSceneFile.h"
#include "CSceneMgr.h"

CScene::CScene()
	:
	m_arrLayer{}
  , m_state(SCENE_STATE::STOP)
  , m_dontDestroyAdded{false}
{
	for (UINT i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]              = new CLayer{};
		m_arrLayer[i]->m_iLayerIdx = i;
	}
}

CScene::~CScene()
{
	Safe_Del_Arr(m_arrLayer);
}


void CScene::start()
{
	for (UINT i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->start();
	}
}

void CScene::update()
{
	for (UINT i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->update();
	}
}

void CScene::lateupdate()
{
	for (UINT i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->lateupdate();
	}
}

void CScene::finalupdate()
{
	CSceneMgr::GetInst()->ClearLayer();
	CRenderMgr::GetInst()->ClearCamera();

	for (UINT i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->finalupdate();
	}
}


void CScene::SetLayerName(int layerIndex, const std::wstring& layerName)
{
	GetLayer(layerIndex)->SetName(layerName);
}


int CScene::GetLayerIdxFromName(const std::wstring& layerName)
{
	const CLayer* pLayer = GetLayer(layerName);
	return (nullptr != pLayer) ? pLayer->GetLayerIdx() : INVALID_LAYER_INDEX;
}

void CScene::AddObject(CGameObject* pRootObject, const std::wstring& _strLayerName)
{
	const int layerIndex = GetLayerIdxFromName(_strLayerName);
	AddObject(pRootObject, layerIndex);
}

void CScene::AddObject(CGameObject* pRootObject, int layerIndex)
{
	assert(nullptr == pRootObject->m_pParent);
	GetLayer(layerIndex)->AddObject(pRootObject);

	// 자식오브젝트들도 해당 레이어의 인덱스를 알려준다.
	std::deque<CGameObject*> queue{};
	queue.push_back(pRootObject);

	// 부모 오브젝트 포함, 자식들 모두 해당 레이어의 인덱스를 알려준다(특정 레이어 소속이 아닌경우에)
	while (false == queue.empty())
	{
		CGameObject* pTarget = queue.front();
		queue.pop_front();

		if (INVALID_LAYER_INDEX == pTarget->m_layerIndex)
			pTarget->m_layerIndex = layerIndex;

		const vector<CGameObject*>& vecChild = pTarget->GetChild();
		for (auto& pChild : vecChild)
		{
			queue.push_back(pChild);
		}
	}
}

CLayer* CScene::GetLayer(const std::wstring& layerName)
{
	const auto iter = std::find_if(std::begin(m_arrLayer)
	                             , std::end(m_arrLayer)
	                             , [&layerName](const CLayer* pLayer)
	                               {
		                               return layerName == pLayer->GetName();
	                               });

	return (iter != std::end(m_arrLayer)) ? *iter : nullptr;
}

Ptr<CSceneFile> CScene::GetSceneFile()
{
	return CResMgr::GetInst()->FindRes<CSceneFile>(m_resourceKey);
}

void CScene::SetSceneState(SCENE_STATE state)
{
	tEventInfo info{};
	info.eType=EVENT_TYPE::SCENE_STATE_CHANGE;
	info.lParam=(DWORD_PTR)state;

	CEventMgr::GetInst()->AddEvent(info);
}

CGameObject* CScene::FindObjectByName(const std::wstring& name)
{
	for (int i = 0; i < std::size(m_arrLayer); ++i)
	{
		CGameObject* pGameObject = FindObject(i, name);
		if (nullptr != pGameObject)
		{
			return pGameObject;
		}
	}
	return nullptr;
}

CGameObject* CScene::FindObject(const std::wstring& layerName, const std::wstring& objectName)
{
	CLayer* pLayer = GetLayer(layerName);
	assert(nullptr!=pLayer);
	auto& vecAllObjects = pLayer->GetObjects();
	auto  iter          = std::find_if(vecAllObjects.begin()
	                                 , vecAllObjects.end()
	                                 , [&objectName](const CGameObject* pGameObject)
	                                   {
		                                   return pGameObject->GetName() == objectName;
	                                   });

	return (iter != vecAllObjects.end()) ? (*iter) : nullptr;
}

CGameObject* CScene::FindObject(int layerIndex, const std::wstring& objectName)
{
	const CLayer* pLayer = GetLayer(layerIndex);
	assert(nullptr!=pLayer);
	return FindObject(pLayer->GetName(), objectName);
}
