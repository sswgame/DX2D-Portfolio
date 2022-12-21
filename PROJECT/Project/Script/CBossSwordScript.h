#pragma once
#include <Engine/CScript.h>

class CBossSwordScript final
	: public CScript
{
public:
	CBossSwordScript();
	virtual ~CBossSwordScript();
	CLONE(CBossSwordScript);

public:
	void start() override;
	void update() override;

private:
	Vec2  m_direction;
	float m_accTime;
	bool  m_isMoving;

	CGameObject* m_pPlayer;
};
