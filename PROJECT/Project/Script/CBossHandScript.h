#pragma once
#include <Engine/CScript.h>

enum class BOSS_HAND_TYPE
{
	LEFT,
	RIGHT,
	END
};

enum class BOSS_HAND_STATE
{
	NONE,
	IDLE,
	CHASE,
	ATTACK
};

class CBossHandScript final
	: public CScript
{
public:
	CBossHandScript();
	virtual ~CBossHandScript();
	CLONE(CBossHandScript);

public:
	void start() override;
	void update() override;

public:
	void Attack() { m_state = BOSS_HAND_STATE::IDLE; }
	void SetHandType(BOSS_HAND_TYPE type) { m_handType = type; }

private:
	BOSS_HAND_TYPE  m_handType;
	BOSS_HAND_STATE m_state;
	float           m_accTime;
	float           m_attackRange;

	std::wstring m_keyPrefabLaser;
	bool         m_isOnLaser;

	CGameObject* m_pPlayer;
};
