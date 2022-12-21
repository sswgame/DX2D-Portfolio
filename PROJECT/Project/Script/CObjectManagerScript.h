#pragma once
#include <Engine/CMgrScript.h>

class CObjectManagerScript final
	: public CMgrScript<CObjectManagerScript>
{
public:
	CObjectManagerScript();
	virtual ~CObjectManagerScript();
	CLONE(CObjectManagerScript);

public:
	void start() override;
public:
	void RegisterPrefab(const std::wstring& prefabKey, UINT count = 10);

	void         ReturnToPool(CGameObject* pGameObject);
	CGameObject* GetObjectFromPool(const std::wstring& prefabKey);

public:
	CGameObject* GetPlayer() const { return m_pPlayer; }
	CGameObject* GetCamera() const { return m_pCamera; }
	CGameObject* GetMouse() const { return m_pMouse; }
private:
	CGameObject* m_pMouse;
	CGameObject* m_pPlayer;
	CGameObject* m_pCamera;

	std::map<std::wstring, std::deque<CGameObject*>> m_mapObjectQueue;
};
