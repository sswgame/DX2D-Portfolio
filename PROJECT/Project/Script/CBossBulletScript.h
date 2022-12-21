#pragma once
#include <Engine/CScript.h>

class CBossBulletScript final
	: public CScript
{
public:
	CBossBulletScript();
	virtual ~CBossBulletScript();
	CLONE(CBossBulletScript);

public:
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void SetDirection(const Vec2& direction) { m_direction = direction; }

private:
	Vec2  m_direction;
	float m_accTime;
	bool  m_isDead;
};
