#include "pch.h"
#include "CPlayerInfoUIScript.h"

#include <Engine/CCamera.h>
#include <Engine/CCollider2D.h>
#include <Engine/CMgrScript.h>

#include "CHPBarScript.h"
#include "CInventoryUIScript.h"
#include "CPlayerScript.h"
#include "CUIManagerScript.h"

CPlayerInfoUIScript::CPlayerInfoUIScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::PLAYERINFOUISCRIPT), DEFINE_NAME(CPlayerInfoUIScript)}
  , m_pDashUI{nullptr}
  , m_pDashUIScript{nullptr}
  , m_pPlayer{nullptr}
  , m_pPlayerScript{nullptr}
  , m_pCamera{nullptr}
  , m_pLifeUI{nullptr}
  , m_pHPScript{nullptr}
  , m_pInfoUI{nullptr}
  , m_pWeaponUI{nullptr}
  , m_pWeaponUIScript{nullptr} {}

CPlayerInfoUIScript::~CPlayerInfoUIScript() = default;

void CPlayerInfoUIScript::start()
{
	m_pPlayer       = CSceneMgr::GetInst()->FindObjectByName(L"Player");
	m_pPlayerScript = static_cast<CPlayerScript*>(m_pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));

	m_pCamera = CSceneMgr::GetInst()->FindObjectByName(L"Main_Camera");
	FindAllChild();
}

void CPlayerInfoUIScript::update()
{
	UpdatePosition();
}

void CPlayerInfoUIScript::HideWeaponUI(bool hide)
{
	if (true == hide)
	{
		m_pWeaponUI->Deactivate();
	}
	else
	{
		m_pWeaponUI->Activate();
	}
}


void CPlayerInfoUIScript::SwapWeaponSlot() const
{
	if (m_pWeaponUI->IsActive())
	{
		m_pWeaponUIScript->SwapSlot();
	}
}

void CPlayerInfoUIScript::UpdatePosition()
{
	const Vec3 cameraPosition = m_pCamera->Transform()->GetRelativePos();
	Transform()->SetRelativePos(cameraPosition);
}

void CPlayerInfoUIScript::FindAllChild()
{
	auto& vecChild = GetOwner()->GetChild();
	for (auto& pChild : vecChild)
	{
		if (L"LIFE_UI" == pChild->GetName())
		{
			m_pLifeUI          = pChild;
			auto& vecLifeChild = pChild->GetChild();
			for (auto& pGameObject : vecLifeChild)
			{
				if (L"BACKGROUND" == pGameObject->GetName())
				{
					m_pHPScript = static_cast<CHPBarScript*>(pGameObject->
						GetScript(static_cast<UINT>(SCRIPT_TYPE::HPBARSCRIPT)));
				}
			}
		}
		else if (L"DASH_UI" == pChild->GetName())
		{
			m_pDashUI       = pChild;
			m_pDashUIScript = static_cast<CDashUIScript*>(pChild->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::DASHUISCRIPT)));
		}
		else if (L"INFO_UI" == pChild->GetName())
		{
			m_pDashUI = pChild;
		}
		else if (L"WEAPON_UI" == pChild->GetName())
		{
			m_pWeaponUI       = pChild;
			m_pWeaponUIScript = static_cast<CWeaponUIScript*>(pChild->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::WEAPONUISCRIPT)));
		}
	}
}
