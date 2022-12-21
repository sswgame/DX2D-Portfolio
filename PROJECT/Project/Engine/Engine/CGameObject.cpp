#include "pch.h"
#include "CGameObject.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CEventMgr.h"

#include "CComponent.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CCollider2D.h"
#include "CLight2D.h"
#include "CRenderComponent.h"


#include "CScript.h"


CGameObject::CGameObject()
	:
	m_arrCom{}
  , m_pRenderComponent{nullptr}
  , m_pParent{nullptr}
  , m_layerIndex{INVALID_LAYER_INDEX}
  , m_bDead{false}
  , m_bActive{true}
  , m_bDynamicShadow{false}
  , m_bFrustumCulling{false} {}

CGameObject::CGameObject(const CGameObject& _origin)
	:
	CEntity(_origin)
  , m_arrCom{}
  , m_pRenderComponent(nullptr)
  , m_pParent(nullptr)
  , m_layerIndex(INVALID_LAYER_INDEX)
  , m_bDead(false)
  , m_bActive(true)
  , m_bDynamicShadow{false}
  , m_bFrustumCulling{false}
{
	for (UINT i = 0; i < std::size(_origin.m_arrCom); ++i)
	{
		if (nullptr != _origin.m_arrCom[i])
		{
			AddComponent(_origin.m_arrCom[i]->Clone());
		}
	}

	for (size_t i = 0; i < _origin.m_vecScript.size(); ++i)
	{
		AddComponent(_origin.m_vecScript[i]->Clone());
	}

	for (size_t i = 0; i < _origin.m_vecChild.size(); ++i)
	{
		AddChild(_origin.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Arr(m_arrCom);
	Safe_Del_Vec(m_vecScript);
	Safe_Del_Vec(m_vecChild);
}

void CGameObject::start()
{
	for (UINT i = 0; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->start();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->start();
	}


	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->start();
	}
}

void CGameObject::update()
{
	for (UINT i = 0; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i] && m_arrCom[i]->IsActive())
			m_arrCom[i]->update();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		if (m_vecScript[i]->IsActive())
			m_vecScript[i]->update();
	}


	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		if (m_vecChild[i]->IsActive())
			m_vecChild[i]->update();
	}
}

void CGameObject::lateupdate()
{
	for (UINT i = 0; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i] && m_arrCom[i]->IsActive())
			m_arrCom[i]->lateupdate();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		if (m_vecScript[i]->IsActive())
			m_vecScript[i]->lateupdate();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		if (m_vecChild[i]->IsActive())
			m_vecChild[i]->lateupdate();
	}
}

void CGameObject::finalupdate()
{
	for (UINT i = 0; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finalupdate();
	}

	// Layer 에 등록
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer    = pCurScene->GetLayer(m_layerIndex);
	pLayer->RegisterObject(this);

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finalupdate();
	}
}

void CGameObject::render()
{
	if (m_pRenderComponent->IsActive())
		m_pRenderComponent->render();
}

CScript* CGameObject::GetScript(UINT scriptIndex)
{
	auto iter = std::find_if(m_vecScript.begin()
	                       , m_vecScript.end()
	                       , [scriptIndex](const CScript* pScript)
	                         {
		                         return pScript->GetScriptType() == scriptIndex;
	                         });
	return (iter != m_vecScript.end()) ? *iter : nullptr;
}

CScript* CGameObject::GetScriptByName(const wstring& scriptName)
{
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		if (m_vecScript[i]->GetName() == scriptName)
			return m_vecScript[i];
	}

	return nullptr;
}


void CGameObject::active()
{
	m_bActive = true;
	for (UINT i = 1; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->active();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->active();
	}


	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->active();
	}
}

void CGameObject::deactive()
{
	m_bActive = false;
	for (UINT i = 1; i < std::size(m_arrCom); ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->deactive();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->deactive();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->deactive();
	}
}

void CGameObject::Deregister()
{
	if (INVALID_LAYER_INDEX == m_layerIndex) { return; }

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pCurLayer = pCurScene->GetLayer(m_layerIndex);
	pCurLayer->DeregisterObject(this);
}

void CGameObject::DisconnectBetweenParent()
{
	assert(m_pParent);

	const auto iter = std::find_if(m_pParent->m_vecChild.begin()
	                             , m_pParent->m_vecChild.end()
	                             , [this](const CGameObject* pGameObject)
	                               {
		                               return this == pGameObject;
	                               });
	if (iter != m_pParent->m_vecChild.end())
	{
		m_pParent->m_vecChild.erase(iter);
		m_pParent = nullptr;
	}
}

void CGameObject::Activate()
{
	tEventInfo info = {};

	info.eType  = EVENT_TYPE::ACTIVATE_OBJECT;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}

void CGameObject::Deactivate()
{
	tEventInfo info = {};

	info.eType  = EVENT_TYPE::DEACTIVATE_OBJECT;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}

bool CGameObject::IsAncestor(CGameObject* pTarget)
{
	const CGameObject* pGameObject = m_pParent;

	while (pGameObject)
	{
		if (pGameObject == pTarget)
			return true;

		pGameObject = pGameObject->m_pParent;
	}

	return false;
}

void CGameObject::AddChild(CGameObject* pChild)
{
	const int layerIndex = pChild->m_layerIndex;

	// 자식으로 들어오는 오브젝트가 루트 오브젝트이고, 특정 레이어 소속이라면
	if (nullptr == pChild->GetParent()
	    && INVALID_LAYER_INDEX != layerIndex)
	{
		// 레이어에서 루트 오브젝트로서 등록 해제
		pChild->Deregister();
		pChild->m_layerIndex = layerIndex;
	}

	// 다른 부모오브젝트가 이미 있다면
	if (pChild->GetParent())
	{
		pChild->DisconnectBetweenParent();
	}

	if (INVALID_LAYER_INDEX == pChild->m_layerIndex)
	{
		pChild->m_layerIndex = m_layerIndex;
	}

	m_vecChild.push_back(pChild);
	pChild->m_pParent = this;
}

void CGameObject::AddComponent(CComponent* pComponent)
{
	const COMPONENT_TYPE type = pComponent->GetType();

	if (COMPONENT_TYPE::SCRIPT != type)
	{
		assert(nullptr == m_arrCom[static_cast<UINT>(type)]);

		m_arrCom[static_cast<UINT>(type)] = pComponent;
		pComponent->m_pOwner              = this;

		switch (pComponent->GetType())
		{
		case COMPONENT_TYPE::MESHRENDER:
		case COMPONENT_TYPE::TILEMAP:
		case COMPONENT_TYPE::PARTICLESYSTEM:
			//case COMPONENT_TYPE::LANDSCAPE:
			//case COMPONENT_TYPE::DECAL:
			{
				// 하나의 오브젝트에 Render 기능을 가진 컴포넌트는 2개이상 들어올 수 없다.
				assert(nullptr == m_pRenderComponent);
				m_pRenderComponent = static_cast<CRenderComponent*>(pComponent);
		}
			break;
		}
	}
	else
	{
		m_vecScript.push_back(static_cast<CScript*>(pComponent));
		pComponent->m_pOwner = this;
	}
}

void CGameObject::DeleteComponent(COMPONENT_TYPE type)
{
	tEventInfo info{};
	info.eType  = EVENT_TYPE::DELETE_COMPONENT;
	info.lParam = (DWORD_PTR)this;
	info.wParam = static_cast<DWORD_PTR>(type);

	CEventMgr::GetInst()->AddEvent(info);
}

void CGameObject::Destroy()
{
	if (m_bDead) { return; }

	tEventInfo info{};
	info.eType  = EVENT_TYPE::DELETE_OBJ;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}


#include "CCamera.h"
#include "CCollider2D.h"
//#include "CCollider3D.h"
#include "CAnimator2D.h"
//#include "CAnimator3D.h"
#include "CParticleSystem.h"
#include "CTileMap.h"

void CGameObject::DeleteScript(int scriptID)
{
	tEventInfo info{};
	info.lParam = (DWORD_PTR)this;
	info.eType  = EVENT_TYPE::DELETE_SCRIPT;
	info.wParam = static_cast<DWORD_PTR>(scriptID);
	CEventMgr::GetInst()->AddEvent(info);
}

void CGameObject::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);
	fwrite(&m_bActive, sizeof(BYTE), 3, _pFile);

	// Component 저장
	for (int i = 0; i < static_cast<int>(COMPONENT_TYPE::END); ++i)
	{
		if (nullptr != m_arrCom[i])
		{
			SaveWStringToFile(ToWString(static_cast<COMPONENT_TYPE>(i)), _pFile);
			m_arrCom[i]->SaveToScene(_pFile);
		}
	}
	SaveWStringToFile(L"END", _pFile);
}

void CGameObject::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);
	fread(&m_bActive, sizeof(BYTE), 3, _pFile);

	// Component 불러오기
	wstring strComponentName;

	while (true)
	{
		LoadWStringFromFile(strComponentName, _pFile);
		if (strComponentName == L"END" || strComponentName.empty())
			break;

		if (strComponentName == ToWString(COMPONENT_TYPE::TRANSFORM))
		{
			AddComponent(new CTransform);
			Transform()->LoadFromScene(_pFile);
		}
		else if (strComponentName == ToWString(COMPONENT_TYPE::CAMERA))
		{
			AddComponent(new CCamera);
			Camera()->LoadFromScene(_pFile);
		}
		else if (strComponentName == ToWString(COMPONENT_TYPE::COLLIDER2D))
		{
			AddComponent(new CCollider2D);
			Collider2D()->LoadFromScene(_pFile);
		}
		//else if (strComponentName == ToWString(COMPONENT_TYPE::COLLIDER3D)) { }
		else if (strComponentName == ToWString(COMPONENT_TYPE::ANIMATOR2D))
		{
			AddComponent(new CAnimator2D);
			Animator2D()->LoadFromScene(_pFile);
		}
		//else if (strComponentName == ToWString(COMPONENT_TYPE::ANIMATOR3D)) { }
		else if (strComponentName == ToWString(COMPONENT_TYPE::LIGHT2D))
		{
			AddComponent(new CLight2D);
			Light2D()->LoadFromScene(_pFile);
		}
		//else if (strComponentName == ToWString(COMPONENT_TYPE::LIGHT3D)) { }


		//else if (strComponentName == ToWString(COMPONENT_TYPE::BOUNDINGBOX)) { }
		else if (strComponentName == ToWString(COMPONENT_TYPE::MESHRENDER))
		{
			AddComponent(new CMeshRender);
			MeshRender()->LoadFromScene(_pFile);
		}
		else if (strComponentName == ToWString(COMPONENT_TYPE::PARTICLESYSTEM))
		{
			AddComponent(new CParticleSystem);
			ParticleSystem()->LoadFromScene(_pFile);
		}
		else if (strComponentName == ToWString(COMPONENT_TYPE::TILEMAP))
		{
			AddComponent(new CTileMap);
			TileMap()->LoadFromScene(_pFile);
		}
		//else if (strComponentName == ToWString(COMPONENT_TYPE::LANDSCAPE)) { }
		//else if (strComponentName == ToWString(COMPONENT_TYPE::DECAL)) { }
	}
}
