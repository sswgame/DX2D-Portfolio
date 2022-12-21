#include "pch.h"
#include "CWeaponUIScript.h"

#include <Engine/CMgrScript.h>

#include "CInventoryUIScript.h"
#include "CItemHolderScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUIManagerScript.h"

CWeaponUIScript::CWeaponUIScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::WEAPONUISCRIPT), DEFINE_NAME(CWeaponUIScript)}
  , m_pPlayer{nullptr}
  , m_pFirstSlot{nullptr}
  , m_pSecondSlot{nullptr}
  , m_pCurrentSlot{nullptr}
  , m_isFinishSwap{false}
  , m_swapTime{0.5f}
  , m_accTime{0}
  , m_swapSlot{false} {}

CWeaponUIScript::~CWeaponUIScript() = default;

void CWeaponUIScript::start()
{
	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();

	FindAllChild();
}

void CWeaponUIScript::update()
{
	if (m_swapSlot && XMVector2NearEqual(Vec2{m_diffPos.x, m_diffPos.y}, Vec2{0.f, 0.f}, Vec2{0.001f, 0.001f}))
	{
		m_firstSlotPos  = m_pFirstSlot->Transform()->GetRelativePos();
		m_secondSlotPos = m_pSecondSlot->Transform()->GetRelativePos();

		m_diffPos      = m_secondSlotPos - m_firstSlotPos;
		m_isFinishSwap = false;
	}

	if (m_swapSlot && false == m_isFinishSwap)
	{
		Vec3 diff = Vec3{m_diffPos.x, m_diffPos.y, m_diffPos.z} / m_swapTime * DT;
		m_pFirstSlot->Transform()->SetRelativePos(m_pFirstSlot->Transform()->GetRelativePos() + diff);
		m_pSecondSlot->Transform()->SetRelativePos(m_pSecondSlot->Transform()->GetRelativePos() - diff);

		m_accTime += DT;

		if (m_accTime >= m_swapTime)
		{
			if (m_pCurrentSlot->GetChild().size() != 0)
			{
				m_pCurrentSlot->GetChild()[0]->Deactivate();
			}
			m_pCurrentSlot = (m_pCurrentSlot == m_pFirstSlot) ? m_pSecondSlot : m_pFirstSlot;
			if (m_pCurrentSlot->GetChild().size() != 0)
			{
				m_pCurrentSlot->GetChild()[0]->Activate();
			}
			m_accTime      = 0.f;
			m_isFinishSwap = true;
			m_diffPos      = Vec3{};
			m_swapSlot     = false;
		}
	}
}

//아이콘을 가져와서 WEAPON UI에서 보여준다
void CWeaponUIScript::active()
{
	CScript::active();
	CGameObject*        pInventory = CMgrScript<CUIManagerScript>::GetInst()->GetUI(UI_TYPE::INVENTORY);
	CInventoryUIScript* pScript    = static_cast<CInventoryUIScript*>(pInventory->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::INVENTORYUISCRIPT)));

	const int    currentWeaponIndex = pScript->GetCurrentEquipIndex();
	CGameObject* pFirstEquipSlot    = pScript->GetFirstEquipSlot();
	//자식으로 아이콘을 가지고 있는지 확인
	if (pFirstEquipSlot->GetChild().size() != 0)
	{
		CGameObject* pIcon = pFirstEquipSlot->GetChild()[0];
		CSceneMgr::GetInst()->AddChild(m_pFirstSlot, pIcon);
		if (0 == currentWeaponIndex)
		{
			pIcon->Activate();
		}
		else
		{
			pIcon->Deactivate();
		}
	}
	CGameObject* pSecondEquipSlot = pScript->GetSecondEquipSlot();
	//자식으로 아이콘을 가지고 있는지 확인
	if (pSecondEquipSlot->GetChild().size() != 0)
	{
		CGameObject* pIcon = pSecondEquipSlot->GetChild()[0];
		CSceneMgr::GetInst()->AddChild(m_pSecondSlot, pIcon);
		if (1 == currentWeaponIndex)
		{
			pIcon->Activate();
		}
		else
		{
			pIcon->Deactivate();
		}
	}

	const Vec3 firstPos  = m_pFirstSlot->Transform()->GetRelativePos();
	const Vec3 secondPos = m_pSecondSlot->Transform()->GetRelativePos();

	if (0 == currentWeaponIndex)
	{
		if (m_pCurrentSlot != m_pFirstSlot)
		{
			m_pFirstSlot->Transform()->SetRelativePos(secondPos);
			m_pSecondSlot->Transform()->SetRelativePos(firstPos);
			m_pCurrentSlot = m_pFirstSlot;
		}
	}
	else
	{
		if (m_pCurrentSlot != m_pSecondSlot)
		{
			m_pFirstSlot->Transform()->SetRelativePos(secondPos);
			m_pSecondSlot->Transform()->SetRelativePos(firstPos);

			m_pCurrentSlot = m_pSecondSlot;
		}
	}
}

//인벤토리로 ICON을 돌려준다.
void CWeaponUIScript::deactive()
{
	CScript::deactive();

	CGameObject*        pInventory = CMgrScript<CUIManagerScript>::GetInst()->GetUI(UI_TYPE::INVENTORY);
	CInventoryUIScript* pScript    = static_cast<CInventoryUIScript*>(pInventory->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::INVENTORYUISCRIPT)));


	//자식으로 아이콘을 가지고 있는지 확인
	if (m_pFirstSlot->GetChild().size() != 0)
	{
		CGameObject* pFirstEquipSlot = pScript->GetFirstEquipSlot();
		CGameObject* pIcon           = m_pFirstSlot->GetChild()[0];
		CSceneMgr::GetInst()->AddChild(pFirstEquipSlot, pIcon);
		pIcon->Activate();
	}

	//자식으로 아이콘을 가지고 있는지 확인
	if (m_pSecondSlot->GetChild().size() != 0)
	{
		CGameObject* pSecondEquipSlot = pScript->GetSecondEquipSlot();
		CGameObject* pIcon            = m_pSecondSlot->GetChild()[0];
		CSceneMgr::GetInst()->AddChild(pSecondEquipSlot, pIcon);
		pIcon->Activate();
	}
}

void CWeaponUIScript::SwapSlot()
{
	m_swapSlot = true;
}

void CWeaponUIScript::FindAllChild()
{
	auto& vecChild = GetOwner()->GetChild();
	m_pFirstSlot   = vecChild[0];
	m_pSecondSlot  = vecChild[1];
	m_pCurrentSlot = m_pFirstSlot;
}
