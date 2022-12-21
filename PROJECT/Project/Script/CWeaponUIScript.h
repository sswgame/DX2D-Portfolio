#pragma once
#include <Engine/CScript.h>

class CWeaponUIScript final
	: public CScript
{
public:
	CWeaponUIScript();
	virtual ~CWeaponUIScript();
	CLONE(CWeaponUIScript);

public:
	void start() override;
	void update() override;

	void active() override;
	void deactive() override;
public:
	void SwapSlot();
private:
	void FindAllChild();
private:
	CGameObject* m_pPlayer;

	CGameObject* m_pFirstSlot;
	CGameObject* m_pSecondSlot;
	CGameObject* m_pCurrentSlot;

	Vec3 m_firstSlotPos;
	Vec3 m_secondSlotPos;
	Vec3 m_diffPos;

	bool m_isFinishSwap;

	float m_swapTime;
	float m_accTime;
	bool  m_swapSlot;
};
