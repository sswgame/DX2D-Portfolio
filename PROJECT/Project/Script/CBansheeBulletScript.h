#pragma once
#include <Engine/CScript.h>

enum class BULLET_STATE
{
	MOVE,
	ON_ATTACK
};

class CBansheeBulletScript final
	: public CScript
{
public:
	CBansheeBulletScript();
	virtual ~CBansheeBulletScript();
	CLONE(CBansheeBulletScript);

public:
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void SetDirection(const Vec2& direction) { m_direction = direction; }
	void ResetObject();

private:
	Vec2         m_direction;
	float        m_destroyTime;
	float        m_speed;
	float        m_accTime;
	BULLET_STATE m_state;
};
