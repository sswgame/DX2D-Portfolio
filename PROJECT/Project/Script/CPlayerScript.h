#pragma once
#include <Engine/CScript.h>

#include "CItemHolderScript.h"
#include "NO_SCRIPT/Define.h"

class CInventoryUIScript;
class CPlayerInfoUIScript;
class CMouseScript;
class CUnitScript;

class CPlayerScript final
	: public CScript
{
public:
	CPlayerScript();
	virtual ~CPlayerScript();
	CLONE(CPlayerScript)

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void start() override;
	void update() override;
	void UpdateData() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;
public:
	COSTUME_TYPE GetCostumeType() const { return m_costumeType; }

	bool IsHalfTilePass() const { return m_isHalfTilePass; }
	void SetHalfTile(bool enable) { m_isHalfTilePass = enable; }
	bool IsJump() const { return m_isJump; }
	bool IsDashState() const { return m_isDash; }
	bool GetRemainTime() const { return m_remainDashTime; }
	int  GetDashCount() const { return m_dashCount; }
	int  GetMaxDashCount() const { return m_maxDashCount; }
	void Jump();
	void Dash();


	void OnAttacked();
	void ChangeCostume(COSTUME_TYPE type);

	CGameObject* GetHand() const { return m_pWeaponHand; }
	CPlayerInfoUIScript* GetPlayerUI() const { return m_pPlayerInfoUIScript; }
	CInventoryUIScript* GetInventory() const { return m_pInventoryUIScript; }

	void SetKeyBoardEnable(bool enable) { m_keyBoardEnable = enable; }
private:
	void UpdatePosition();
	void UpdateAction();
	void FindAllChild();
	void SetState();
	void SetDirection();
	void ShowDustEffect();
private:
	CUnitScript* m_pUnitScript;
	CMouseScript* m_pMouseScript;

	CPlayerInfoUIScript* m_pPlayerInfoUIScript;
	CInventoryUIScript* m_pInventoryUIScript;

	CGameObject* m_pWeaponHand;
	CItemHolderScript* m_pItemHolderScript;
	std::vector<CGameObject*> m_vecHands;

	bool  m_isHalfTilePass;
	float m_accTime;
	bool  m_isCostumeChanged;

	int   m_maxDashCount;
	int   m_dashCount;
	float m_dashCoolTime;
	float m_remainDashTime;

	bool m_isDash;
	bool m_showDustEffect;
	bool m_isJump;
	int  m_isAttacked;

	std::wstring m_keyPrefabAfterImage;
	std::wstring m_keyPrefabWarning;

	float        m_speed;
	COSTUME_TYPE m_costumeType;

	CGameObject* m_pDustEffect;

	bool m_keyBoardEnable;
};
