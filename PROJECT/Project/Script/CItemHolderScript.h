#pragma once
#include <Engine/CMgrScript.h>
#include <Engine/CScript.h>

#include "CCameraScript.h"
#include "CInventoryUIScript.h"
#include "CUIManagerScript.h"
#include "CWeaponScript.h"

class CWeaponScript;

class CItemHolderScript final
	: public CScript
{
public:
	CItemHolderScript();
	virtual ~CItemHolderScript();
	CLONE(CItemHolderScript);

public:
	void start() override;
	void update() override;

public:
	void SetWeapon(CGameObject* pItem, int weaponSlotIndex);
	void Attack(const Vec3& direction, const Vec3& rotation);
	bool IsAttack() const { return m_isOnAttack; }
	Vec3 GetWeaponRotation() const { return (m_attackIndex == 1) ? Vec3{0.f, 0.f, XM_PI} : Vec3{0.f, 0.f, 0.f}; }

	CWeaponScript* GetWeapon() const { return m_pCurrentWeaponScript; }
	void           ChangeWeaponSlot();
private:
	void ShakeCamera() const { m_pCameraScript->ShakeCamera(0.2f); }
private:
	CGameObject*   m_pCurrentWeapon;
	CWeaponScript* m_pCurrentWeaponScript;

	std::array<CGameObject*, 2> m_arrWeaponSlot;
	int                         m_currentWeaponIndex;

	CCameraScript* m_pCameraScript;


	bool  m_isOnAttack;
	int   m_attackIndex;
	bool  m_finishAttack;
	float m_accTime;

	int m_attackDirection;
};
