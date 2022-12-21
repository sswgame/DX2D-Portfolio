#pragma once
#include <array>
#include <Engine/CScript.h>
class CMouseScript;
class CPlayerScript;

enum class SLOT_TYPE
{
	WEAPON,
	INVENTORY,
	ACCESSORY,
};

class CInventoryUIScript final
	: public CScript
{
public:
	CInventoryUIScript();
	virtual ~CInventoryUIScript();
	CLONE(CInventoryUIScript);

public:
	void start() override;
	void update() override;

	void deactive() override;
public:
	void ChangeEquipSlot();
	void UpdateAccessory();

	void         InsertItem(CGameObject* pRealItem, CGameObject* pIcon);
	CGameObject* GetFirstEquipSlot() const { return m_arrEquipWeapon[0][0]; }
	CGameObject* GetSecondEquipSlot() const { return m_arrEquipWeapon[1][0]; }
	int          GetCurrentEquipIndex() const { return m_currentWeaponSlotIndex; }
private:
	void UpdatePosition();
	void CheckMousePosition();
	void UpdateCopiedIconPosition();

	//Init
	void FindAllChild();
	void SetDefaultItem();
	//Check
	void CheckInventory(CGameObject* pMouse);
	void CheckEquipSlot(CGameObject* pMouse);
	void CheckAccessory(CGameObject* pMouse);
	void CheckCloseButton(CGameObject* pMouse);

	void EquipItem(CGameObject* pItemSlot, int itemSlotIndex, int weaponSlotIndex);
	void UnEquipItem(CGameObject* pSlot, int weaponSlotIndex);
	void RemoveRealItem(int validItemSlotIndex, int weaponSlotIndex);
	void EquipRealItem(int itemSlotIndex, int weaponSlotIndex, bool activate = true);
	void SwapRealItem(int itemSlotIndex, int weaponSlotIndex);

	bool         HasItem(CGameObject* pSlot, SLOT_TYPE type);
	CGameObject* GetIcon(CGameObject* pSlot, SLOT_TYPE type);
private:
	int m_dragWeaponIndex;
	int m_dragAccessoryIndex;
	int m_dragInventoryIndex;

	bool m_isMouseHoverItem;

	std::array<CGameObject*, 15> m_arrRealItemSlot;
	std::array<CGameObject*, 2>  m_arrRealEquipSlot;

	std::array<CGameObject*, 15>               m_arrIconSlot;
	std::array<CGameObject*, 4>                m_arrAccessorySlot;
	std::array<std::array<CGameObject*, 2>, 2> m_arrEquipWeapon;
	std::array<CGameObject*, 2>                m_arrEquipSlot;
	CGameObject*                               m_pCloseButton;

	int   m_currentWeaponSlotIndex;
	float m_currentWeaponSlotCoolTime;

	CPlayerScript* m_pPlayerScript;
	CMouseScript* m_pMouseScript;

	CGameObject* m_pCopiedIcon;
};
