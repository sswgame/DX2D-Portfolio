#include "pch.h"
#include "CGravityScript.h"

#include "CScriptMgr.h"

#include "CPlayerScript.h"

float CGravityScript::s_gravity = -750.f;

namespace
{
	constexpr float FRICTION_AMOUNT = 500.f;
}

CGravityScript::CGravityScript()
	:
	CScript{static_cast<UINT>(SCRIPT_TYPE::GRAVITYSCRIPT), DEFINE_NAME(CGravityScript)}
  , m_isOnGravity(false)
  , m_maxSpeedX{600.f}
  , m_maxSpeedY{600.f} {}

CGravityScript::~CGravityScript() = default;

void CGravityScript::update()
{
	if (true == m_isOnGravity)
	{
		m_force += Vec2{0.f, s_gravity};
	}

	const Vec2 acceleration = m_force / 1.f;
	m_velocity += acceleration * DT;

	// 최대 속도 축별 제한

	m_velocity.x = std::clamp(m_velocity.x, -m_maxSpeedX, m_maxSpeedX);
	m_velocity.y = std::clamp(m_velocity.y, -m_maxSpeedY, m_maxSpeedY);
	//if (m_velocity.x != 0.f)
	{
		// 좌우 마찰력 설정
		Vec2 friction = Vec2{-m_velocity.x, 0.f};
		friction.Normalize();
		friction *= FRICTION_AMOUNT * DT;

		if (m_velocity.Length() < friction.Length())
		{
			m_velocity.x = 0.f;
			m_velocity.y = 0.f;
		}
		else
		{
			m_velocity += friction;
		}
	}

	const Vec2 move     = m_velocity * DT;
	const Vec3 position = Transform()->GetRelativePos();
	Transform()->SetRelativePos(position + Vec3{move.x, move.y, 0.f});

	m_force = Vec2{};
}