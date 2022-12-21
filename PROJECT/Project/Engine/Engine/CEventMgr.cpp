#include "pch.h"
#include "CEventMgr.h"

#include "CCollisionMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderMgr.h"
#include "CComponent.h"
#include "CScript.h"

CEventMgr::CEventMgr()
	:
	m_bObjEvn{false}
  , m_change{false}
  , m_sceneChangeInfo{} {}

CEventMgr::~CEventMgr() {}

void CEventMgr::update()
{
	m_bObjEvn = false;
	m_change = false;

	// Dead Object 삭제
	for (size_t i = 0; i < m_vecDead.size(); ++i)
	{
		assert(m_vecDead[i]);

		// 삭제되는 오브젝트가 부모가 있다면(자식 오브젝트라면)
		if (m_vecDead[i]->GetParent())
		{
			m_vecDead[i]->DisconnectBetweenParent();
		}

		delete m_vecDead[i];
		m_bObjEvn = true;
	}
	m_vecDead.clear();


	// Event 처리
	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJ:
			// lParam : Object Adress, wParam : Layer Index
		{
			CGameObject* pGameObject = (CGameObject*)m_vecEvent[i].lParam;
			const int    layerIndex = static_cast<int>(m_vecEvent[i].wParam);
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			pCurScene->AddObject(pGameObject, layerIndex);

			if (SCENE_STATE::PLAY == pCurScene->GetSceneState())
			{
				pGameObject->start();
			}

			m_bObjEvn = true;
		}
		break;

		case EVENT_TYPE::DELETE_OBJ:
			// lParam : Object Adress
		{
			CGameObject* pDeleteObject = (CGameObject*)m_vecEvent[i].lParam;

			if (false == pDeleteObject->m_bDead)
			{
				m_vecDead.push_back(pDeleteObject);

				// 보유하고 있는 자식 오브젝트도 전부 Dead 체크 해둔다.
				static list<CGameObject*> queue;
				queue.clear();

				queue.push_back(pDeleteObject);

				while (!queue.empty())
				{
					CGameObject* pObj = queue.front();
					queue.pop_front();
					pObj->m_bDead = true;

					const vector<CGameObject*>& vecChild = pObj->GetChild();
					for (size_t j = 0; j < vecChild.size(); ++j)
					{
						queue.push_back(vecChild[j]);
					}
				}
			}
		}

		break;

		case EVENT_TYPE::ADD_CHILD:
			// lParam : Parent Object, wParam : Child Object
		{
			CGameObject* pParent = (CGameObject*)m_vecEvent[i].lParam;
			CGameObject* pChild = (CGameObject*)m_vecEvent[i].wParam;

			pParent->AddChild(pChild);
			m_bObjEvn = true;
		}
		break;
		case EVENT_TYPE::CHANGE_LAYER:
		{
			CGameObject* pGameObject = (CGameObject*)(m_vecEvent[i].lParam);
			const int    layerIndex = static_cast<int>(m_vecEvent[i].wParam);
			//루트 오브젝트인 경우, 레이어가 변경되는 것이므로, 변경 레이어의 루트 오브젝트로 등록.
			if (nullptr == pGameObject->GetParent())
			{
				pGameObject->Deregister();
				const CScene* pCurrentScene = CSceneMgr::GetInst()->GetCurScene();
				pCurrentScene->GetLayer(layerIndex)->RegisterObjectAsRoot(pGameObject);
			}
			//자식오브젝트인경우, 어차피 eventMgr이 돈 이후에, 모든 레이어가 정리되고, 다시 세팅되므로
			//별다른 작업을 해줄 이유가 없다.
			pGameObject->m_layerIndex = layerIndex;
		}
		break;

		case EVENT_TYPE::DELETE_COMPONENT:
		{
			CGameObject* pGameObject = (CGameObject*)(m_vecEvent[i].lParam);
			COMPONENT_TYPE type = static_cast<COMPONENT_TYPE>(m_vecEvent[i].wParam);

			if (COMPONENT_TYPE::SCRIPT != type)
			{
				assert(nullptr != pGameObject->m_arrCom[static_cast<UINT>(type)]);

				delete pGameObject->m_arrCom[static_cast<UINT>(type)];
				pGameObject->m_arrCom[static_cast<UINT>(type)] = nullptr;

				switch (type)
				{
				case COMPONENT_TYPE::MESHRENDER:
				case COMPONENT_TYPE::TILEMAP:
				case COMPONENT_TYPE::PARTICLESYSTEM:
					//case COMPONENT_TYPE::LANDSCAPE:
					//case COMPONENT_TYPE::DECAL:
				{
					// 하나의 오브젝트에 Render 기능을 가진 컴포넌트는 2개이상 들어올 수 없다.
					assert(nullptr != pGameObject->m_pRenderComponent);
					pGameObject->m_pRenderComponent = nullptr;
				}
				break;
				}
			}
		}
		break;

		case EVENT_TYPE::DELETE_SCRIPT:
		{
			CGameObject* pGameObject = (CGameObject*)(m_vecEvent[i].lParam);
			int          scriptID = static_cast<int>(m_vecEvent[i].wParam);
			auto         iter = std::find_if(pGameObject->m_vecScript.begin()
				, pGameObject->m_vecScript.end()
				, [scriptID](const CScript* pScript)
				{
					return pScript->GetScriptType() == scriptID;
				});
			if (iter != pGameObject->m_vecScript.end())
			{
				delete (*iter);
				pGameObject->m_vecScript.erase(iter);
			}
		}
		break;

		case EVENT_TYPE::DISCONNECT_PARENT:
			// lParam : Parent Object, wParam : Child Object
		{
			CGameObject* pObject = (CGameObject*)m_vecEvent[i].lParam;
			pObject->DisconnectBetweenParent();
			const int     layerIndex = pObject->GetLayerIndex();
			const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			pCurScene->GetLayer(layerIndex)->RegisterObjectAsRoot(pObject);

			m_bObjEvn = true;
		}
		break;


		case EVENT_TYPE::SET_CAMEAR_INDEX:
		{
			CCamera* cam = (CCamera*)m_vecEvent[i].lParam;
			int      iChangeIdx = static_cast<int>(m_vecEvent[i].wParam);

			CRenderMgr::GetInst()->SwapCameraIndex(cam, iChangeIdx);
		}
			break;

		case EVENT_TYPE::ACTIVATE_OBJECT:
		{
			CGameObject* pObject = (CGameObject*)m_vecEvent[i].lParam;
			pObject->active();
		}
			break;

		case EVENT_TYPE::DEACTIVATE_OBJECT:
		{
			CGameObject* pObject = (CGameObject*)m_vecEvent[i].lParam;
			pObject->deactive();
		}
		break;


		case EVENT_TYPE::ACTIVATE_COMPONENT:
		{
			CComponent* pCom = (CComponent*)m_vecEvent[i].lParam;
			pCom->active();
		}
		break;

		case EVENT_TYPE::DEACTIVATE_COMOPNENT:
		{
			CComponent* pCom = (CComponent*)m_vecEvent[i].lParam;
			pCom->deactive();
		}
		break;

		case EVENT_TYPE::SCENE_CHANGE:
		{
			m_bObjEvn = true;
			if (false == m_change)
			{
				m_sceneChangeInfo = m_vecEvent[i];
				m_change = true;
			}
		}
		break;

		case EVENT_TYPE::SCENE_STATE_CHANGE:
		{
			m_bObjEvn = true;
			SCENE_STATE state = (SCENE_STATE)m_vecEvent[i].lParam;
			CScene* pCurrentScene = CSceneMgr::GetInst()->GetCurScene();
			if (pCurrentScene->GetSceneState() == state) { return; }

			if (SCENE_STATE::STOP == pCurrentScene->GetSceneState())
			{
				if (SCENE_STATE::PLAY == state)
				{
					pCurrentScene->m_state = state;
					pCurrentScene->start();

					if (false == pCurrentScene->IsDonDestroyAdded())
					{
						const auto& vecDontDestroy = CSceneMgr::GetInst()->GetListDontDestroy();
						for (auto& pTarget : vecDontDestroy)
						{
							CLayer* pLayer = pCurrentScene->GetLayer(pTarget->GetLayerIndex());
							pLayer->AddObject(pTarget);
						}
						pCurrentScene->SetAddDonDestroy(true);
					}
				}
			}
			else if (SCENE_STATE::PLAY == pCurrentScene->GetSceneState())
			{
				if (SCENE_STATE::PAUSE == state)
				{
					pCurrentScene->m_state = state;
				}
				else if (SCENE_STATE::STOP == state)
				{
					pCurrentScene->m_state = state;
					assert(CResMgr::GetInst()->FindRes<CSceneFile>(pCurrentScene->GetResKey()).Get());
				}
				else if(SCENE_STATE::RESUME == state)
				{
					if (false == pCurrentScene->IsDonDestroyAdded())
					{
						const auto& vecDontDestroy = CSceneMgr::GetInst()->GetListDontDestroy();
						for (auto& pTarget : vecDontDestroy)
						{
							CLayer* pLayer = pCurrentScene->GetLayer(pTarget->GetLayerIndex());
							pLayer->AddObject(pTarget);
						}
					}
				}
			}
		}
			break;
		case EVENT_TYPE::DELETE_RES:
		{
			// lParam : Res Adress
			CRes* pRes = (CRes*)m_vecEvent[i].lParam;

			CResMgr::GetInst()->DeleteRes(pRes->GetKey());

			m_bObjEvn = true;
		}
		break;
		case EVENT_TYPE::REMOVE_FROM_SCENE:
		{
			CGameObject* pGameObject = (CGameObject*)m_vecEvent[i].lParam;
			pGameObject->SetInScene(false);
			if (pGameObject->GetParent())
			{
				pGameObject->DisconnectBetweenParent();
			}

			if (pGameObject->Collider2D())
			{
				CCollisionMgr::GetInst()->DeRegisterObject(pGameObject);
			}

			CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(pGameObject->GetLayerIndex());

			auto& vecRoot = pLayer->GetRootObjects();
			auto  iter    = std::find_if(vecRoot.begin()
			                           , vecRoot.end()
			                           , [pGameObject](const CGameObject* pObject)
				                             {
					                             return pObject == pGameObject;
				                             });
			if (iter != vecRoot.end())
			{
				vecRoot.erase(iter);
			}
			auto& vecAllObjects = pLayer->GetObjects();
			iter                = std::find_if(vecAllObjects.begin()
			                                 , vecAllObjects.end()
			                                 , [pGameObject](const CGameObject* pObject)
				                                   {
					                                   return pObject == pGameObject;
				                                   });
			if (iter != vecAllObjects.end())
			{
				vecAllObjects.erase(iter);
			}
			
		}
		break;
		}
	}
	m_vecEvent.clear();

	if (m_change)
	{
		CScene* pLoadScene = (CScene*)m_sceneChangeInfo.lParam;
		bool keep = (bool)m_sceneChangeInfo.wParam;
		CSceneMgr::GetInst()->ChangeScene(pLoadScene, keep);
		m_sceneChangeInfo = {};
	}
}
