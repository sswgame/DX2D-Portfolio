#include "pch.h"
#include "CObjectManagerScript.h"

#include <Engine/CCollisionMgr.h>
#include <Engine/CEventMgr.h>
#include "CSceneSaveLoad.h"
#include "NO_SCRIPT/Define.h"

CObjectManagerScript::CObjectManagerScript()
	:
	m_pMouse{nullptr}
  , m_pPlayer{nullptr}
  , m_pCamera{nullptr} {}

CObjectManagerScript::~CObjectManagerScript()
{
	for (auto& [prefabKey,queue] : m_mapObjectQueue)
	{
		while (false == queue.empty())
		{
#ifdef _DEBUG
			OutputDebugString(prefabKey.c_str());
			OutputDebugString(L"\n");
#endif
			CGameObject* pGameObject = queue.front();
			SAFE_DELETE(pGameObject);
			queue.pop_front();
		}
	}
	m_mapObjectQueue.clear();
}

void CObjectManagerScript::start()
{
	m_pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");
	m_pCamera = CSceneMgr::GetInst()->FindObjectByName(L"Main_Camera");
	m_pMouse  = CSceneMgr::GetInst()->FindObjectByName(L"Mouse");

	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(DEFAULT_LAYER, L"DEFAULT");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(TILE_LAYER, L"Tile");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(PLAYER_LAYER, L"Player");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(PLAYER_ATTACK_LAYER, L"PlayerAttack");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_LAYER, L"Monster");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_ATTACK_LAYER, L"MonsterAttack");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_SPAWN_LAYER, L"MonsterSpawner");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(ENTER_SCENE_LAYER, L"EnterScene");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(UI_LAYER, L"UI");


	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Tile");
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Monster");
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"MonsterAttack");
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"EnterScene");

	CCollisionMgr::GetInst()->CollisionCheck(L"Monster", L"Tile");
	CCollisionMgr::GetInst()->CollisionCheck(L"Monster", L"PlayerAttack");
	CCollisionMgr::GetInst()->CollisionCheck(L"MonsterAttack", L"Tile");
}


void CObjectManagerScript::RegisterPrefab(const std::wstring& prefabKey, UINT count)
{
	auto iter = m_mapObjectQueue.find(prefabKey);
	if (iter != m_mapObjectQueue.end()) { return; }

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(prefabKey);

	std::deque<CGameObject*> queueObjects{};
	for (UINT i = 0; i < count; ++i)
	{
		CGameObject* pGameObject = pPrefab->Instantiate();
		queueObjects.push_back(pGameObject);
	}

	m_mapObjectQueue.insert({prefabKey, queueObjects});
}

void CObjectManagerScript::ReturnToPool(CGameObject* pGameObject)
{
	std::wstring prefabKey = L"prefab\\" + pGameObject->GetName() + L".pref";
	auto         iter      = m_mapObjectQueue.find(prefabKey);
	assert(iter!=m_mapObjectQueue.end());

	iter->second.push_back(pGameObject);
}

CGameObject* CObjectManagerScript::GetObjectFromPool(const std::wstring& prefabKey)
{
	auto iter = m_mapObjectQueue.find(prefabKey);
	if (iter == m_mapObjectQueue.end())
	{
		RegisterPrefab(prefabKey);
		iter = m_mapObjectQueue.find(prefabKey);
	}

	if (iter->second.empty())
	{
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(prefabKey);

		for (int i = 0; i < 10; ++i)
		{
			CGameObject* pGameObject = pPrefab->Instantiate();
			iter->second.push_back(pGameObject);
		}
	}

	CGameObject* pGameObject = iter->second.front();
	iter->second.pop_front();

	return pGameObject;
}
