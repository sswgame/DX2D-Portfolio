#pragma once
#include "CEntity.h"

#include "CSceneFile.h"

class CLayer;
class CGameObject;

class CScene :
	public CEntity
{
	friend class CEventMgr;
	friend class CSceneMgr;
private:
	CLayer*      m_arrLayer[MAX_LAYER];
	SCENE_STATE  m_state;
	std::wstring m_resourceKey;
	bool m_dontDestroyAdded;
public:
	void start();
	void update();
	void lateupdate();
	void finalupdate();


public:
	void SetAddDonDestroy(bool enable){m_dontDestroyAdded=enable;}
	bool IsDonDestroyAdded() const{return m_dontDestroyAdded;}

	void SetLayerName(int layerIndex, const std::wstring& layerName);
	void AddObject(CGameObject* pRootObject, const std::wstring& _strLayerName);
	void AddObject(CGameObject* pRootObject, int layerIndex);

	CLayer* GetLayer(int layerIndex) const
	{
		assert(0<=layerIndex && layerIndex <std::size(m_arrLayer));
		return m_arrLayer[layerIndex];
	}

	CLayer* GetLayer(const std::wstring& layerName);

	void            SetResKey(const std::wstring& _strKey) { m_resourceKey = _strKey; SetName(_strKey);}
	const std::wstring& GetResKey() const {return m_resourceKey;}
	Ptr<CSceneFile> GetSceneFile();

	SCENE_STATE GetSceneState() const { return m_state; }
	void        SetSceneState(SCENE_STATE state);

	CGameObject* FindObjectByName(const std::wstring& name);
	CGameObject* FindObject(const std::wstring& layerName, const std::wstring& objectName);
	CGameObject* FindObject(int layerIndex, const std::wstring& objectName);
private:
	int GetLayerIdxFromName(const std::wstring& layerName);

	CLONE(CScene);
public:
	CScene();
	virtual ~CScene();
};
