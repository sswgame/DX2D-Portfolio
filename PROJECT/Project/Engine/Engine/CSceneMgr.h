#pragma once

class CScene;
class CGameObject;

class CSceneMgr
	: public CSingleton<CSceneMgr>
{
	friend class CSingleton<CSceneMgr>;
	friend class CEventMgr;
private:
	CSceneMgr();
	virtual ~CSceneMgr();

private:
	CScene*                   m_pCurScene;
	std::vector<CGameObject*> m_vecDontDestroy;
	std::unordered_map<std::wstring,CScene*> m_mapScene;

	bool m_drawCollider;
private:
	void  ChangeScene(CScene* _pNextScene,bool keep);
public:
	void GoToNextScene(CScene* pNextScene,bool keep=false);

	CScene* GetCurScene() const { return m_pCurScene; }
	void SpawnObject(CGameObject* _pSpawnObject, Vec3 _vWorldPos, wstring _strName, UINT _iLayerIdx);
	void SpawnObject(CGameObject* _pSpawnObject, UINT _iLayerIdx);
	void AddChild(CGameObject* _pParentObject, CGameObject* _pChildObject);
	void DisconnectParent(CGameObject* _pObject);
	void ChangeObjectLayer(CGameObject* pGameObject, int layerIndex);
	void ClearLayer();
	void DontDestroyOnLoad(CGameObject* pGameObject);
	void ClearDonDestroyOnLoad() { m_vecDontDestroy.clear(); };
	const std::vector<CGameObject*>& GetListDontDestroy() const {return m_vecDontDestroy;}
	void RemoveFromSceneButNotDelete(CGameObject* pGameObject);

	CGameObject*              FindObjectByName(const wstring& _strName);
	void                      FindObjectsByName(const wstring& _strName, vector<CGameObject*>& _vecOut);
	std::vector<CGameObject*> GetAllObjects();

	bool IsDrawCollider() const { return m_drawCollider; }
	void RenderCollider(bool enable) { m_drawCollider = enable; }
public:
	void init();
	void progress();
};
