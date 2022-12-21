#include "pch.h"
#include "CToolObjMgr.h"

#include "CGameObjectEx.h"
#include "CCameraEx.h"
#include <Engine/CTransform.h>

#include <Engine/CRenderMgr.h>
#include "CEditorCamScript.h"

CToolObjMgr::CToolObjMgr() {}

CToolObjMgr::~CToolObjMgr()
{
	Safe_Del_Vec(m_vecObj);
}

void CToolObjMgr::init()
{
	// Editor Camera
	CGameObjectEx* pToolObj = new CGameObjectEx{};
	pToolObj->SetName(L"EditorCamera");
	pToolObj->AddComponent(new CTransform{});
	pToolObj->AddComponent(new CCameraEx{});
	pToolObj->AddComponent(new CEditorCamScript{});

	pToolObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pToolObj->Camera()->SetFar(10000);
	pToolObj->Camera()->CheckLayerMaskAll();

	m_vecObj.push_back(pToolObj);

	CRenderMgr::GetInst()->RegisterEditorCamera(pToolObj->Camera());
}

void CToolObjMgr::progress()
{
	if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() == SCENE_STATE::PLAY) return;

	for (const auto& pGameObjectEx : m_vecObj)
	{
		pGameObjectEx->update();
	}
	for (const auto& pGameObjectEx : m_vecObj)
	{
		pGameObjectEx->lateupdate();
	}
	for (const auto& pGameObjectEx : m_vecObj)
	{
		pGameObjectEx->finalupdate();
	}
	for (const auto& pGameObjectEx : m_vecObj)
	{
		if (nullptr == pGameObjectEx->GetRenderComponent()
		    || nullptr == pGameObjectEx->GetRenderComponent()->GetMesh()
		    || nullptr == pGameObjectEx->GetRenderComponent()->GetMaterial()
		    || nullptr == pGameObjectEx->GetRenderComponent()->GetMaterial()->GetShader())
		{
			continue;
		}

		pGameObjectEx->render();
	}
}
