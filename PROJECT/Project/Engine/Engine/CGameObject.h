#pragma once
#include "CEntity.h"

#define GET_COMPONENT(type, TYPE)  class C##type* type() { return (C##type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CComponent;
class CTransform;
class CCollider2D;
class CAnimator2D;
class CCamera;
class CLight2D;
//class CLight3D;
class CMeshRender;
class CTimeMap;
class CParticleSystem;
class CScript;
class CRenderComponent;

class CGameObject :
	public CEntity
{
private:
	vector<CGameObject*> m_vecChild;
	vector<CScript*>     m_vecScript;

	CComponent*       m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
	CRenderComponent* m_pRenderComponent;

	CGameObject* m_pParent;
	int          m_layerIndex; // 게임 오브젝트 소속 레이어 인덱스   
	bool         m_bDead;

	bool m_bActive;
	bool m_bDynamicShadow;   // 동적 그림자 생성
	bool m_bFrustumCulling;  // 절두체 컬링 사용 유무

	bool m_isInScene = false;
public:
	void start();
	void update();
	void lateupdate();
	void finalupdate();
	void render();

private:
	void active();
	void deactive();

public:
	void                        SetInScene(bool enable) { m_isInScene = enable; }
	bool                        IsInScene() const { return m_isInScene; }
	CGameObject*                GetParent() const { return m_pParent; }
	const vector<CGameObject*>& GetChild() { return m_vecChild; }

	// Deregister ==> 등록 취소(등록->미등록)
	// Unregister ==> 등록 안됨(등록 x == 등록->미등록, 애초에 등록된적 없음)
	void Deregister();
	void DisconnectBetweenParent();
	void Activate();
	void Deactivate();

	bool IsDead() const { return m_bDead; }
	bool IsActive() const { return m_bActive; }
	bool IsAncestor(CGameObject* pTarget);

public:
	void              AddChild(CGameObject* pChild);

	void              AddComponent(CComponent* pComponent);
	CComponent*       GetComponent(COMPONENT_TYPE _eType) const { return m_arrCom[static_cast<UINT>(_eType)]; }
	void              DeleteComponent(COMPONENT_TYPE type);

	CRenderComponent* GetRenderComponent() const { return m_pRenderComponent; }

	void Destroy();

	GET_COMPONENT(Transform, TRANSFORM)
	GET_COMPONENT(MeshRender, MESHRENDER)
	GET_COMPONENT(Camera, CAMERA)
	GET_COMPONENT(Collider2D, COLLIDER2D)
	GET_COMPONENT(Animator2D, ANIMATOR2D)
	GET_COMPONENT(TileMap, TILEMAP)
	GET_COMPONENT(ParticleSystem, PARTICLESYSTEM)
	GET_COMPONENT(Light2D, LIGHT2D)

	const vector<CScript*>& GetScripts() { return m_vecScript; }
	CScript*                GetScript(UINT scriptIndex);
	CScript*                GetScriptByName(const wstring& scriptName);

	int  GetLayerIndex() const { return m_layerIndex; }
	void SetLayerIndex(int layerIndex) { m_layerIndex = layerIndex; }

	template <typename T>
	T* GetScript();

	void DeleteScript(int scriptID);
public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CGameObject)

public:
	CGameObject();
	CGameObject(const CGameObject& _origin);
	virtual ~CGameObject();

	friend class CEventMgr;
	friend class CScene;
	friend class CLayer;
};

template <typename T>
T* CGameObject::GetScript()
{
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		T* pScript = dynamic_cast<T*>(m_vecScript[i]);
		if (nullptr != pScript)
			return pScript;
	}

	return nullptr;
}
