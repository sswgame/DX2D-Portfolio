#pragma once
#include <Engine/CScript.h>

enum class BAT_BULLET_STATE
{
	STOP,
	MOVE,
	ATTACK
};

class CBatBulletScript final
	: public CScript
{
public:
	CBatBulletScript();
	virtual ~CBatBulletScript();
	CLONE(CBatBulletScript);

public:
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void             Move();
	void             ResetObject();
	void             SetDirection(const Vec2& direction) { m_bulletDirection = direction; }
	bool             IsMoving() const { return m_state == BAT_BULLET_STATE::MOVE; }
	BAT_BULLET_STATE GetState() const { return m_state; }
	bool             IsReturned() const { return m_isReturned; }
	void             SetReturn(bool enable) { m_isReturned = enable; }
private:
	Vec2             m_bulletDirection;
	float            m_accTime;
	float            m_speed;
	BAT_BULLET_STATE m_state;
	bool             m_isAttack;
	bool             m_isReturned = false;
};
