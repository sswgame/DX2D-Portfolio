#pragma once
#include <Engine/CScript.h>

#include "CGravityScript.h"
#include "NO_SCRIPT/Define.h"

class CUnitScript final
	: public CScript
{
public:
	CUnitScript();
	virtual ~CUnitScript();
	CLONE(CUnitScript);

	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void start() override;
	void update() override;
	void lateupdate() override;
public:
	void UpdateAnimation();
	void SetRepeat(bool enable) { m_repeatAnimation = enable; }
	void SetAnimation(UNIT_STATE state, const std::wstring& animationName);
	void ResetAnimationList() { m_mapAnimation.clear(); }

	void SetGround(bool ground) { m_isGround = ground; }
	bool IsGround() const { return m_isGround; }

	void           SetUnitDirection(UNIT_DIRECTION direction);
	UNIT_DIRECTION GetUnitDirection() const { return m_direction; }

	void SetHP(int hp) { m_hp = hp; }
	int  GetHP() const { return m_hp; }
	void UpdateHP(int damage) { m_hp -= damage; }

	void SetMaxHP(int hp) { m_maxHP = hp; }
	int  GetMaxHP() const { return m_maxHP; }

	void       SetUnitState(UNIT_STATE state) { m_state = state; }
	UNIT_STATE GetUnitState() const { return m_state; }

	bool            HasGravityScript() const { return m_pGravityScript != nullptr; }
	CGravityScript* GetGravityScript() const { return m_pGravityScript; }
	void            RegisterGravityScript(CGravityScript* pGravityScript) { m_pGravityScript = pGravityScript; }

private:
	UNIT_DIRECTION  m_direction;
	UNIT_STATE      m_state;
	bool            m_isGround;
	CGravityScript* m_pGravityScript;

	int                                                m_hp;
	int                                                m_maxHP;
	bool                                               m_repeatAnimation;
	std::unordered_map<UNIT_STATE, const std::wstring> m_mapAnimation;
};
