#include "pch.h"
#include "CInventoryUIScript.h"

#include <Engine/CCamera.h>

#include "CMouseScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerInfoUIScript.h"
#include "CPlayerScript.h"

namespace
{
	const wchar_t* SHORT_SWORD = L"prefab\\ShortSword.pref";

	const wchar_t* G_WEAPONS[] =
	{
		L"prefab\\CosmosSword.pref"
	  , L"prefab\\KeresScythe.pref"
	};

	constexpr int INVALID_SLOT_INDEX = -1;

	inline const wchar_t* WEAPON_EQUIP = L"sound\\WEAPON_EQUIP.wav";
}

CInventoryUIScript::CInventoryUIScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::INVENTORYUISCRIPT), DEFINE_NAME(CInventoryUIScript)}
  , m_dragWeaponIndex{INVALID_SLOT_INDEX}
  , m_dragAccessoryIndex{INVALID_SLOT_INDEX}
  , m_dragInventoryIndex{INVALID_SLOT_INDEX}
  , m_isMouseHoverItem{false}
  , m_arrRealItemSlot{}
  , m_arrRealEquipSlot{}
  , m_arrIconSlot{}
  , m_arrAccessorySlot{}
  , m_arrEquipWeapon{}
  , m_arrEquipSlot{}
  , m_pCloseButton{nullptr}
  , m_currentWeaponSlotIndex{0}
  , m_currentWeaponSlotCoolTime{0}
  , m_pPlayerScript{nullptr}
  , m_pMouseScript{nullptr}
  , m_pCopiedIcon{nullptr} {}

CInventoryUIScript::~CInventoryUIScript() = default;

void CInventoryUIScript::start()
{
	FindAllChild();

	CGameObject* pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
	m_pPlayerScript = static_cast<CPlayerScript*>(pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));
	SetDefaultItem();

	m_pPlayerScript->GetPlayerUI()->GetOwner()->Activate();
	GetOwner()->Deactivate();

	CGameObject* pMouse = CMgrScript<CObjectManagerScript>::GetInst()->GetMouse();
	m_pMouseScript      = static_cast<CMouseScript*>(pMouse->GetScript((UINT)SCRIPT_TYPE::MOUSESCRIPT));

	Vec3 pos = Transform()->GetRelativePos();
	Transform()->SetRelativePos(Vec3{pos.x, pos.y, -10.f});
}

void CInventoryUIScript::update()
{
	UpdatePosition();
	CheckMousePosition();
	UpdateCopiedIconPosition();
}

void CInventoryUIScript::deactive()
{
	CScript::deactive();

	//인벤토리가 꺼질 때 이벤트 큐에 만약 자식들이 activate하는 이벤트가 들어있으면 그걸 덮어씌워서 다 꺼지도록 해야한다.
	auto& vecChild = GetOwner()->GetChild();
	for (auto& pChild : vecChild)
	{
		pChild->Deactivate();
	}
}

void CInventoryUIScript::ChangeEquipSlot()
{
	if (HasItem(m_arrEquipWeapon[m_currentWeaponSlotIndex][0], SLOT_TYPE::WEAPON))
	{
		CItemHolderScript* pItemHolderScript = static_cast<CItemHolderScript*>(m_pPlayerScript->GetHand()->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));
		pItemHolderScript->GetWeapon()->GetOwner()->Deactivate();
	}

	m_currentWeaponSlotIndex = (++m_currentWeaponSlotIndex) % m_arrEquipSlot.size();

	if (GetOwner()->IsActive())
	{
		m_arrEquipSlot[m_currentWeaponSlotIndex]->Activate();
	}
}

void CInventoryUIScript::UpdateAccessory() {}

void CInventoryUIScript::InsertItem(CGameObject* pRealItem, CGameObject* pIcon)
{
	auto iter = std::find_if(m_arrRealItemSlot.begin()
	                       , m_arrRealItemSlot.end()
	                       , [](const CGameObject* pItemSlot)
		                         {
			                         return nullptr == pItemSlot;
		                         });
	if (iter != m_arrRealItemSlot.end())
	{
		const size_t index = std::distance(m_arrRealItemSlot.begin(), iter);
		//아이콘 이동
		CSceneMgr::GetInst()->AddChild(m_arrIconSlot[index], pIcon);

		//실제 무기 이동
		m_arrRealItemSlot[index] = pRealItem;
		m_arrRealItemSlot[index]->Deactivate();
	}
}

void CInventoryUIScript::UpdatePosition()
{
	const Vec3 cameraPosition = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera()->Transform()->GetRelativePos();
	const float width = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera()->Camera()->GetWidth();
	Vec3 position = Transform()->GetRelativePos();
	position.x = cameraPosition.x + width * .5f - 290.f;
	position.y = cameraPosition.y;
	Transform()->SetRelativePos(position);
}

void CInventoryUIScript::CheckMousePosition()
{
	m_isMouseHoverItem = false;
	CheckInventory(CMgrScript<CObjectManagerScript>::GetInst()->GetMouse());
	CheckEquipSlot(CMgrScript<CObjectManagerScript>::GetInst()->GetMouse());
	CheckAccessory(CMgrScript<CObjectManagerScript>::GetInst()->GetMouse());
	CheckCloseButton(CMgrScript<CObjectManagerScript>::GetInst()->GetMouse());
}

void CInventoryUIScript::UpdateCopiedIconPosition()
{
	if (nullptr != m_pCopiedIcon
	    && KEY_PRESSED(KEY::LBTN))
	{
		static CGameObject* pMouse = CMgrScript<CObjectManagerScript>::GetInst()->GetMouse();

		Vec3 mousePose    = pMouse->Transform()->GetRelativePos();
		Vec3 inventoryPos = Transform()->GetRelativePos();
		m_pCopiedIcon->Transform()->SetRelativePos(Vec3{mousePose.x, mousePose.y, inventoryPos.z - 100.f});
	}
	if (nullptr != m_pCopiedIcon
	    && KEY_AWAY(KEY::LBTN))
	{
		if (false == m_isMouseHoverItem)
		{
			m_pCopiedIcon->Destroy();
			m_pCopiedIcon        = nullptr;
			m_dragInventoryIndex = INVALID_SLOT_INDEX;
			m_dragWeaponIndex    = INVALID_SLOT_INDEX;
		}
	}
}

void CInventoryUIScript::FindAllChild()
{
	//Inventory
	auto& vecChild = GetOwner()->GetChild();
	auto  iter     = std::find_if(vecChild.begin()
	                            , vecChild.end()
	                            , [](const CGameObject* pChild)
		                              {
			                              return pChild->GetName() == L"Array_Slot";
		                              });
	assert(iter != vecChild.end());

	auto& vecItemSlots = (*iter)->GetChild();
	for (int i = 0; i < m_arrIconSlot.size(); ++i)
	{
		m_arrIconSlot[i] = vecItemSlots[i];
	}

	//Accessory
	iter = std::find_if(vecChild.begin()
	                  , vecChild.end()
	                  , [](const CGameObject* pChild)
		                    {
			                    return pChild->GetName() == L"Array_Accessory";
		                    });
	assert(iter != vecChild.end());

	auto& vecAccessorySlots = (*iter)->GetChild();
	for (int i = 0; i < m_arrAccessorySlot.size(); ++i)
	{
		m_arrAccessorySlot[i] = vecAccessorySlots[i];
	}

	//Equip Item Slot
	iter = std::find_if(vecChild.begin()
	                  , vecChild.end()
	                  , [](const CGameObject* pChild)
		                    {
			                    return pChild->GetName() == L"Array_Weapon";
		                    });
	assert(iter != vecChild.end());
	auto& vecEquipSlots = (*iter)->GetChild();
	for (int i = 0; i < vecEquipSlots.size(); ++i)
	{
		m_arrEquipSlot[i] = vecEquipSlots[i];
		auto& vecSlots    = vecEquipSlots[i]->GetChild();
		for (int j = 0; j < vecSlots.size(); ++j)
		{
			m_arrEquipWeapon[i][j] = vecSlots[j];
		}
	}

	//Close Button
	iter = std::find_if(vecChild.begin()
	                  , vecChild.end()
	                  , [](const CGameObject* pChild)
		                    {
			                    return pChild->GetName() == L"CloseButton";
		                    });
	assert(iter != vecChild.end());
	m_pCloseButton = (*iter);
}

void CInventoryUIScript::SetDefaultItem()
{
	//1. 플레이어가 최초에 무기가 없는 경우 기본적으로 숏소드를 할당해준다.
	if (0 == m_pPlayerScript->GetHand()->GetChild().size())
	{
		CGameObject* pShortSword = CResMgr::GetInst()->FindRes<CPrefab>(SHORT_SWORD)->Instantiate();
		CSceneMgr::GetInst()->SpawnObject(pShortSword, DEFAULT_LAYER);
		CSceneMgr::GetInst()->AddChild(m_pPlayerScript->GetHand(), pShortSword);

		CItemHolderScript* pItemHolderScript = static_cast<CItemHolderScript*>(m_pPlayerScript->GetHand()->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));
		pItemHolderScript->SetWeapon(pShortSword, m_currentWeaponSlotIndex);

		CWeaponScript* pScript = static_cast<CWeaponScript*>(pShortSword->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::WEAPONSCRIPT)));
		const std::wstring& iconPrefabName = pScript->GetIconName();

		CGameObject* pIcon = CResMgr::GetInst()->FindRes<CPrefab>(iconPrefabName)->Instantiate();
		CSceneMgr::GetInst()->SpawnObject(pIcon, DEFAULT_LAYER);
		CSceneMgr::GetInst()->AddChild(m_arrEquipWeapon[m_currentWeaponSlotIndex][0], pIcon);
		Vec3 pos = pIcon->Transform()->GetRelativePos();
		pos.z    = -70.f;
		pIcon->Transform()->SetRelativePos(pos);
	}
	//2. 나머지 무기를 모두 넣는다
	for (int i = 0; i < std::size(G_WEAPONS); ++i)
	{
		CGameObject* pItem = CResMgr::GetInst()->FindRes<CPrefab>(G_WEAPONS[i])->Instantiate();
		CSceneMgr::GetInst()->SpawnObject(pItem, DEFAULT_LAYER);
		CWeaponScript* pScript = static_cast<CWeaponScript*>(pItem->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::WEAPONSCRIPT)));

		const std::wstring& iconPrefabName = pScript->GetIconName();
		CGameObject*        pIcon          = CResMgr::GetInst()->FindRes<CPrefab>(iconPrefabName)->Instantiate();
		CSceneMgr::GetInst()->SpawnObject(pIcon, DEFAULT_LAYER);
		CSceneMgr::GetInst()->AddChild(m_pPlayerScript->GetOwner(), pItem);
		Vec3 pos = pIcon->Transform()->GetRelativePos();
		pos.z    = -70.f;
		pIcon->Transform()->SetRelativePos(pos);

		InsertItem(pItem, pIcon);
	}
}

void CInventoryUIScript::CheckInventory(CGameObject* pMouse)
{
	for (int i = 0; i < m_arrIconSlot.size(); ++i)
	{
		if (m_pMouseScript->IsMouseIn(m_arrIconSlot[i]))
		{
			m_isMouseHoverItem = true;

			m_arrIconSlot[i]->GetChild()[0]->Activate();
			if (m_pMouseScript->IsClicked(m_arrIconSlot[i], KEY::RBTN))
			{
				EquipItem(m_arrIconSlot[i], i, m_currentWeaponSlotIndex);
			}
			if (m_pMouseScript->IsClicked(m_arrIconSlot[i], KEY::LBTN))
			{
				if (HasItem(m_arrIconSlot[i], SLOT_TYPE::INVENTORY))
				{
					m_dragInventoryIndex = i;
					m_pCopiedIcon        = GetIcon(m_arrIconSlot[i], SLOT_TYPE::INVENTORY)->Clone();
					CSceneMgr::GetInst()->SpawnObject(m_pCopiedIcon, DEFAULT_LAYER);
				}
			}
			if (nullptr != m_pCopiedIcon && KEY_AWAY(KEY::LBTN))
			{
				if (INVALID_SLOT_INDEX != m_dragInventoryIndex)
				{
					if (nullptr != m_arrRealItemSlot[i])
					{
						CSceneMgr::GetInst()->AddChild(m_arrIconSlot[m_dragInventoryIndex]
						                             , GetIcon(m_arrIconSlot[i], SLOT_TYPE::INVENTORY));
					}
					CSceneMgr::GetInst()->AddChild(m_arrIconSlot[i]
					                             , GetIcon(m_arrIconSlot[m_dragInventoryIndex], SLOT_TYPE::INVENTORY));
					std::swap(m_arrRealItemSlot[i], m_arrRealItemSlot[m_dragInventoryIndex]);
					m_pCopiedIcon->Destroy();
					m_pCopiedIcon        = nullptr;
					m_dragInventoryIndex = INVALID_SLOT_INDEX;
				}
				else if (INVALID_SLOT_INDEX != m_dragWeaponIndex)
				{
					UnEquipItem(m_arrEquipWeapon[m_dragWeaponIndex][0], m_dragWeaponIndex);
					m_pCopiedIcon->Destroy();
					m_pCopiedIcon     = nullptr;
					m_dragWeaponIndex = INVALID_SLOT_INDEX;
				}
			}
		}
		else
		{
			if (m_arrIconSlot[i]->GetChild()[0]->IsActive())
				m_arrIconSlot[i]->GetChild()[0]->Deactivate();
		}
	}
}

void CInventoryUIScript::CheckEquipSlot(CGameObject* pMouse)
{
	if (m_arrEquipSlot[1 - m_currentWeaponSlotIndex]->MeshRender()->IsActive())
	{
		m_arrEquipSlot[1 - m_currentWeaponSlotIndex]->MeshRender()->Deactivate();
	}
	for (int i = 0; i < m_arrEquipWeapon.size(); ++i)
	{
		for (int j = 0; j < m_arrEquipWeapon[i].size(); ++j)
		{
			if (m_pMouseScript->IsMouseIn(m_arrEquipWeapon[i][j]))
			{
				m_isMouseHoverItem = true;

				if (false == m_arrEquipWeapon[i][j]->IsActive())
				{
					m_arrEquipWeapon[i][j]->Activate();
				}

				if (m_pMouseScript->IsClicked(m_arrEquipWeapon[i][j], KEY::RBTN))
				{
					UnEquipItem(m_arrEquipWeapon[i][j], i);
				}

				if (m_pMouseScript->IsClicked(m_arrIconSlot[i], KEY::LBTN))
				{
					if (HasItem(m_arrEquipWeapon[i][j], SLOT_TYPE::WEAPON))
					{
						m_dragWeaponIndex = i;
						m_pCopiedIcon     = GetIcon(m_arrEquipWeapon[i][j], SLOT_TYPE::WEAPON)->Clone();
						CSceneMgr::GetInst()->SpawnObject(m_pCopiedIcon, DEFAULT_LAYER);
					}
				}
				if (nullptr != m_pCopiedIcon && KEY_AWAY(KEY::LBTN))
				{
					if (INVALID_SLOT_INDEX != m_dragInventoryIndex)
					{
						EquipItem(m_arrIconSlot[m_dragInventoryIndex], m_dragInventoryIndex, i);

						m_pCopiedIcon->Destroy();
						m_pCopiedIcon        = nullptr;
						m_dragInventoryIndex = INVALID_SLOT_INDEX;
						m_dragWeaponIndex    = INVALID_SLOT_INDEX;
					}
					else if (INVALID_SLOT_INDEX != m_dragWeaponIndex)
					{
						m_pCopiedIcon->Destroy();
						m_pCopiedIcon     = nullptr;
						m_dragWeaponIndex = INVALID_SLOT_INDEX;
					}
				}
			}
			else
			{
				if (HasItem(m_arrEquipWeapon[i][j], SLOT_TYPE::WEAPON))
				{
					if (false == m_arrEquipWeapon[i][j]->IsActive())
						m_arrEquipWeapon[i][j]->Activate();
				}
				else
				{
					if (m_arrEquipWeapon[i][j]->IsActive())
						m_arrEquipWeapon[i][j]->Deactivate();
				}
			}
		}
	}
}

CGameObject* CInventoryUIScript::GetIcon(CGameObject* pSlot, SLOT_TYPE type)
{
	switch (type)
	{
	case SLOT_TYPE::WEAPON:
		return pSlot->GetChild()[0];
	case SLOT_TYPE::INVENTORY:
		return pSlot->GetChild()[1];
	case SLOT_TYPE::ACCESSORY:
	default:
		return nullptr;
	}
}

bool CInventoryUIScript::HasItem(CGameObject* pSlot, SLOT_TYPE type)
{
	switch (type)
	{
	case SLOT_TYPE::WEAPON:
		return pSlot->GetChild().size() != 0 ? true : false;
	case SLOT_TYPE::INVENTORY:
		return 1 < pSlot->GetChild().size() ? true : false;
	case SLOT_TYPE::ACCESSORY:
	default: return false;
	}
}

void CInventoryUIScript::UnEquipItem(CGameObject* pSlot, int weaponSlotIndex)
{
	if (HasItem(pSlot, SLOT_TYPE::WEAPON))
	{
		auto iter = std::find_if(m_arrIconSlot.begin()
		                       , m_arrIconSlot.end()
		                       , [this](CGameObject* pItemSlot)
			                         {
				                         return (false == HasItem(pItemSlot, SLOT_TYPE::INVENTORY));
			                         });
		if (iter != m_arrIconSlot.end())
		{
			//아이콘 이동
			CSceneMgr::GetInst()->AddChild(*iter, GetIcon(pSlot, SLOT_TYPE::WEAPON));
			//실제 무기 이동
			const size_t index = std::distance(m_arrIconSlot.begin(), iter);
			RemoveRealItem(static_cast<int>(index), weaponSlotIndex);
		}
	}
}

void CInventoryUIScript::RemoveRealItem(int validItemSlotIndex, int weaponSlotIndex)
{
	CItemHolderScript* pItemHolder = static_cast<CItemHolderScript*>(m_pPlayerScript->GetHand()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));

	if (m_currentWeaponSlotIndex == weaponSlotIndex)
	{
		m_arrRealItemSlot[validItemSlotIndex] = pItemHolder->GetWeapon()->GetOwner();
		m_arrRealItemSlot[validItemSlotIndex]->Deactivate();

		CSceneMgr::GetInst()->DisconnectParent(pItemHolder->GetWeapon()->GetOwner());
		CSceneMgr::GetInst()->AddChild(m_pPlayerScript->GetOwner(), pItemHolder->GetWeapon()->GetOwner());

		pItemHolder->SetWeapon(nullptr, m_currentWeaponSlotIndex);
	}
	else
	{
		//내부에서 현재 사용 중인 무기를 꺼버린다.
		pItemHolder->ChangeWeaponSlot();


		m_arrRealItemSlot[validItemSlotIndex] = pItemHolder->GetWeapon()->GetOwner();
		m_arrRealItemSlot[validItemSlotIndex]->Deactivate();

		CSceneMgr::GetInst()->DisconnectParent(pItemHolder->GetWeapon()->GetOwner());
		CSceneMgr::GetInst()->AddChild(m_pPlayerScript->GetOwner(), pItemHolder->GetWeapon()->GetOwner());

		pItemHolder->SetWeapon(nullptr, 1 - m_currentWeaponSlotIndex);
		pItemHolder->ChangeWeaponSlot();
		if (weaponSlotIndex != m_currentWeaponSlotIndex)
		{
			pItemHolder->GetWeapon()->GetOwner()->Activate();
		}
	}
}

void CInventoryUIScript::EquipRealItem(int itemSlotIndex, int weaponSlotIndex, bool activate)
{
	CItemHolderScript* pItemHolderScript = static_cast<CItemHolderScript*>(m_pPlayerScript->GetHand()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));

	activate ? m_arrRealItemSlot[itemSlotIndex]->Activate() : m_arrRealItemSlot[itemSlotIndex]->Deactivate();
	if (weaponSlotIndex != m_currentWeaponSlotIndex)
	{
		m_arrRealItemSlot[itemSlotIndex]->Deactivate();
	}

	pItemHolderScript->SetWeapon(m_arrRealItemSlot[itemSlotIndex], weaponSlotIndex);
	CSceneMgr::GetInst()->AddChild(pItemHolderScript->GetOwner(), m_arrRealItemSlot[itemSlotIndex]);

	m_arrRealItemSlot[itemSlotIndex] = nullptr;
}

void CInventoryUIScript::SwapRealItem(int itemSlotIndex, int weaponSlotIndex)
{
	CItemHolderScript* pItemHolderScript = static_cast<CItemHolderScript*>(m_pPlayerScript->GetHand()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));

	CGameObject* pNewWeapon = m_arrRealItemSlot[itemSlotIndex];

	pNewWeapon->Activate();

	RemoveRealItem(itemSlotIndex, weaponSlotIndex);

	pItemHolderScript->SetWeapon(pNewWeapon, m_currentWeaponSlotIndex);
	CSceneMgr::GetInst()->AddChild(pItemHolderScript->GetOwner(), pNewWeapon);
}

void CInventoryUIScript::EquipItem(CGameObject* pItemSlot, int itemSlotIndex, int weaponSlotIndex)
{
	if (HasItem(pItemSlot, SLOT_TYPE::INVENTORY))
	{
		CResMgr::GetInst()->Load<CSound>(WEAPON_EQUIP, WEAPON_EQUIP)->Play(1, 1.f);
		if (HasItem(m_arrEquipWeapon[weaponSlotIndex][0], SLOT_TYPE::WEAPON))
		{
			//아이콘들만 해당
			CSceneMgr::GetInst()->AddChild(pItemSlot
			                             , GetIcon(m_arrEquipWeapon[weaponSlotIndex][0], SLOT_TYPE::WEAPON));

			CSceneMgr::GetInst()->AddChild(m_arrEquipWeapon[weaponSlotIndex][0]
			                             , GetIcon(pItemSlot, SLOT_TYPE::INVENTORY));

			//실제 무기를 옮겨야 한다.
			SwapRealItem(itemSlotIndex, weaponSlotIndex);
		}
		else
		{
			CSceneMgr::GetInst()->AddChild(m_arrEquipWeapon[weaponSlotIndex][0]
			                             , GetIcon(pItemSlot, SLOT_TYPE::INVENTORY));
			EquipRealItem(itemSlotIndex, weaponSlotIndex);
		}
	}
}

void CInventoryUIScript::CheckAccessory(CGameObject* pMouse)
{
	for (const auto& pSlot : m_arrAccessorySlot)
	{
		if (m_pMouseScript->IsMouseIn(pSlot))
		{
			if (false == pSlot->GetChild()[0]->IsActive())
			{
				pSlot->GetChild()[0]->Activate();
			}
			//m_pMouseScript->IsClicked(pSlot, KEY::RBTN);
		}
		else
		{
			if (pSlot->GetChild()[0]->IsActive())
				pSlot->GetChild()[0]->Deactivate();
		}
	}
}

void CInventoryUIScript::CheckCloseButton(CGameObject* pMouse)
{
	if (m_pMouseScript->IsMouseIn(m_pCloseButton))
	{
		if (false == m_pCloseButton->GetChild()[0]->IsActive())
		{
			m_pCloseButton->GetChild()[0]->Activate();
		}

		if (m_pMouseScript->IsClicked(m_pCloseButton, KEY::LBTN))
		{
			GetOwner()->Deactivate();
			m_pPlayerScript->GetPlayerUI()->HideWeaponUI(false);
		}
	}
	else
	{
		if (m_pCloseButton->GetChild()[0]->IsActive())
		{
			m_pCloseButton->GetChild()[0]->Deactivate();
		}
	}
}
