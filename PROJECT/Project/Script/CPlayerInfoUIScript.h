#pragma once
#include <Engine/CScript.h>

#include "CDashUIScript.h"
#include "CHPBarScript.h"
#include "CWeaponUIScript.h"


class CHPBarScript;

class CPlayerInfoUIScript final
	: public CScript
{
public:
	CPlayerInfoUIScript();
	virtual ~CPlayerInfoUIScript();
	CLONE(CPlayerInfoUIScript);

public:
	void start() override;
	void update() override;
public:
	void HideWeaponUI(bool hide);
	void SetDashCount(int count) const { m_pDashUIScript->SetDashCount(count); }
	void SetMaxDashCount(int count) const { m_pDashUIScript->SetMaxDashCount(count); }
	void SwapWeaponSlot() const;

	void SetHP(float hpRatio) { m_pHPScript->SetRemainHP(hpRatio); }
private:
	void UpdatePosition();
	void FindAllChild();
private:
	CGameObject*   m_pDashUI;
	CDashUIScript* m_pDashUIScript;

	CGameObject*   m_pPlayer;
	CPlayerScript* m_pPlayerScript;

	CGameObject* m_pCamera;

	CGameObject*  m_pLifeUI;
	CHPBarScript* m_pHPScript;


	CGameObject*     m_pInfoUI;
	CGameObject*     m_pWeaponUI;
	CWeaponUIScript* m_pWeaponUIScript;
};
