#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer final
	: public CEntity
{
private:
	vector<CGameObject*> m_vecRoot;      // 레이어에 속한 최상위 부모 오브젝트
	vector<CGameObject*> m_vecObj;       // 매 프레임 마다 파악되는 레이어 소속 모든 오브젝트
	int                  m_iLayerIdx;    // Scene 에서의 인덱스


public:
	int GetLayerIdx() const { return m_iLayerIdx; }


public:
	void start();
	void update();
	void lateupdate();
	void finalupdate();


private:
	void AddObject(CGameObject* pGameObject) { m_vecRoot.push_back(pGameObject); }
	void RegisterObjectAsRoot(CGameObject* _pObj) { m_vecRoot.push_back(_pObj); }
	void RegisterObject(CGameObject* _pObj) { m_vecObj.push_back(_pObj); }
	void DeregisterObject(CGameObject* pTarget);
	void Clear() { m_vecObj.clear(); }

public:
	vector<CGameObject*>& GetRootObjects() { return m_vecRoot; }
	vector<CGameObject*>& GetObjects() { return m_vecObj; }

	CLONE_DISABLE(CLayer)

public:
	CLayer();
	virtual ~CLayer();

	friend class CSceneMgr;
	friend class CScene;
	friend class CGameObject;
	friend class CEventMgr;
};
