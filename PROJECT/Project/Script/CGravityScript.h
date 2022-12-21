#pragma once

#include <Engine/CScript.h>

class CGravityScript final
	: public CScript
{
public:
	static void SetGravityIntensity(float gravity) { s_gravity = gravity; }
public:
	CGravityScript();
	virtual ~CGravityScript();
	CLONE(CGravityScript);

	void SaveToScene(FILE* _pFile) override {};
	void LoadFromScene(FILE* _pFile) override {};

public:
	void update() override;

public:
	void SetForce(const Vec2& force) { m_force = force; }

	void        SetVelocity(const Vec2& velocity) { m_velocity = velocity; }
	const Vec2& GetVelocity() const { return m_velocity; }

	void SetGravity(bool enable) { m_isOnGravity = enable; }
	bool IsOnGravity() const { return m_isOnGravity; }

private:
	static float s_gravity;

	Vec2  m_force;
	Vec2  m_velocity;
	bool  m_isOnGravity;
	float m_maxSpeedX;
	float m_maxSpeedY;
};
