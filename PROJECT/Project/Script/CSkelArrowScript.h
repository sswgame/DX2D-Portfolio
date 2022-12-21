#pragma once
#include <Engine/CScript.h>

class CSkelArrowScript final
	: public CScript
{
public:
	CSkelArrowScript();
	virtual ~CSkelArrowScript();
	CLONE(CSkelArrowScript);

public:
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void SetDirection(const Vec2& direction, float radian, bool isLeft)
	{
		m_direction       = direction;
		m_isLeft          = isLeft;
		m_rotationZRadian = radian;
	}

	void ResetObject();
private:
	Vec2 m_direction;

	bool  m_isLeft;
	float m_rotationZRadian;
	float m_speed;
};
