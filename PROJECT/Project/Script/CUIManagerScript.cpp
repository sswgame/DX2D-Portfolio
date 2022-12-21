#include "pch.h"
#include "CUIManagerScript.h"
#include <Engine/CCollider2D.h>

namespace
{
	const wchar_t* g_arrUIName[] =
	{
		L"INVENTORY_UI"
	  , L"PLAYERINFO_UI"
		// , L"ABILITY_UI"
		//  , L"STATUS_UI"
		//  , L"MAP_UI"
		//  , L"MINI_MAP_UI"
		//  , L"DIALOGUE_UI"
		// , L"RESTAURANT_UI"
		//  , L"SHOP_UI"
		//  , L"COSTUME_UI"
	   ,
	};

	constexpr int UI_LAYER_INDEX = 31;
}

CUIManagerScript::CUIManagerScript()
	:
	m_pMouse{nullptr}
  , m_mouseHovered{false} {}

CUIManagerScript::~CUIManagerScript() = default;

void CUIManagerScript::start()
{
	if (m_mapUI.size() == 0)
	{
		for (int i = 0; i < std::size(g_arrUIName); ++i)
		{
			std::wstring key = std::wstring{L"prefab\\"} + g_arrUIName[i] + L".pref";
			CGameObject* pUI = CResMgr::GetInst()->Load<CPrefab>(key, key)->Instantiate();
			CSceneMgr::GetInst()->SpawnObject(pUI, CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"UI")->GetLayerIdx());
			pUI->Deactivate();

			m_mapUI.insert({g_arrUIName[i], pUI});
		}
	}

	m_pMouse = CSceneMgr::GetInst()->FindObjectByName(L"Mouse");
}

CGameObject* CUIManagerScript::GetUI(UI_TYPE type)
{
	auto iter = m_mapUI.find(g_arrUIName[static_cast<UINT>(type)]);
	assert(iter != m_mapUI.end());

	return iter->second;
}
