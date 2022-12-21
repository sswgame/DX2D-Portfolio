#include "pch.h"
#include "InspectorUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CScript.h>

#include "Animator2DUI.h"
#include "ScriptUI.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "CameraUI.h"
#include "Collider2DUI.h"
#include "Light2DUI.h"


#include "MaterialUI.h"
#include "ParticleSystemUI.h"
#include "TextureUI.h"
#include "TileMapUI.h"

InspectorUI::InspectorUI()
	:
	UI("Inspector")
  , m_pTargetObject(nullptr)
  , m_pTargetRes(nullptr)
  , m_arrComUI{}
  , m_arrResUI{}
{
	SetImGuiWindowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	// ComponentUI 생성   
	ComponentUI* pComUI{};

	pComUI = new TransformUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)] = pComUI;

	pComUI = new CameraUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::CAMERA)] = pComUI;

	pComUI = new Collider2DUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER2D)] = pComUI;

	pComUI = new Light2DUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LIGHT2D)] = pComUI;

	pComUI = new Animator2DUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::ANIMATOR2D)] = pComUI;

	pComUI = new MeshRenderUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::MESHRENDER)] = pComUI;

	pComUI = new TileMapUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TILEMAP)] = pComUI;

	pComUI = new ParticleSystemUI{};
	AddChild(pComUI);
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::PARTICLESYSTEM)] = pComUI;
	// ==============
	// ResInfoUI 생성
	// ==============
	ResInfoUI* pResInfoUI{};

	// MaterialUI
	pResInfoUI = new MaterialUI{};
	AddChild(pResInfoUI);
	m_arrResUI[static_cast<UINT>(RES_TYPE::MATERIAL)] = pResInfoUI;

	// TextureUI
	pResInfoUI = new TextureUI{};
	AddChild(pResInfoUI);
	m_arrResUI[static_cast<UINT>(RES_TYPE::TEXTURE)] = pResInfoUI;
}

InspectorUI::~InspectorUI() {}


void InspectorUI::update()
{
	if (KEY_TAP(KEY::I) && false == KEY_TYPING_ENABLED)
	{
		if (IsActive())
			Deactivate();
		else
			Activate();
	}
	UI::update();
	
}

void InspectorUI::render_update() {}


void InspectorUI::SetTargetObject(CGameObject* _pTarget)
{
	m_pTargetObject = _pTarget;

	for (int i = 0; i < std::size(m_arrComUI); ++i)
	{
		if (nullptr != m_arrComUI[i])
		{
			// Object 가 nullptr 인 경우
			if (nullptr == m_pTargetObject)
			{
				m_arrComUI[i]->Deactivate();
				m_arrComUI[i]->SetTargetObject(nullptr);
			}
			else if (m_pTargetObject->GetComponent(static_cast<COMPONENT_TYPE>(i)))
			{
				m_arrComUI[i]->Activate();
				m_arrComUI[i]->SetTargetObject(m_pTargetObject);
			}
			else
			{
				m_arrComUI[i]->Deactivate();
			}
		}
	}

	// 오브젝트가 null 이면
	if (nullptr == m_pTargetObject)
	{
		// 모든 스크립트UI 비활성화
		for (const auto& pScriptUI : m_vecScriptUI)
		{
			pScriptUI->Deactivate();
		}
	}
	else
	{
		const vector<CScript*>& vecScripts = m_pTargetObject->GetScripts();
		ScriptUI*               pScriptUI{};

		for (size_t i = 0; i < vecScripts.size(); ++i)
		{
			if (m_vecScriptUI.size() <= i)
				pScriptUI = AddScriptUI();
			else
				pScriptUI = m_vecScriptUI[i];

			pScriptUI->SetTargetObject(m_pTargetObject);
			pScriptUI->SetTargetScript(vecScripts[i]);
			pScriptUI->Activate();
		}

		// ScriptUI 가 더 많이 있을때
		if (vecScripts.size() < m_vecScriptUI.size())
		{
			// 대응하는 UI 를 제외한 나머지 ScriptUI 들을 비활성화 한다.ㄴ
			for (size_t i = vecScripts.size(); i < m_vecScriptUI.size(); ++i)
			{
				m_vecScriptUI[i]->Deactivate();
			}
		}
	}


	// ResInfoUI 비활성화
	for (int i = 0; i < std::size(m_arrResUI); ++i)
	{
		if (nullptr != m_arrResUI[i] && m_arrResUI[i]->IsActive())
			m_arrResUI[i]->Deactivate();
	}
}

void InspectorUI::SetTargetResource(CRes* _pTargetRes)
{
	// ComponentUI 전부 비활성화
	for (int i = 0; i < std::size(m_arrComUI); ++i)
	{
		if (nullptr != m_arrComUI[i] && m_arrComUI[i]->IsActive())
			m_arrComUI[i]->Deactivate();
	}

	// ScriptUI 전부 비활성화
	for (const auto& pScriptUI : m_vecScriptUI)
	{
		pScriptUI->Deactivate();
	}

	// 활성화 시킬 RES_TYPE 을 알아냄
	RES_TYPE type = _pTargetRes->GetResType();

	// 해당 리소스 ResInfoUI 활성화
	for (int i = 0; i < std::size(m_arrResUI); ++i)
	{
		if (nullptr != m_arrResUI[i])
		{
			if (i == static_cast<int>(type))
			{
				m_arrResUI[i]->Activate();
				m_arrResUI[i]->SetTargetRes(_pTargetRes);
			}
			else
				m_arrResUI[i]->Deactivate();
		}
	}
}

ScriptUI* InspectorUI::AddScriptUI()
{
	ScriptUI* pScriptUI = new ScriptUI{};
	pScriptUI->Deactivate();
	AddChild(pScriptUI);

	m_vecScriptUI.push_back(pScriptUI);

	return pScriptUI;
}
