#include "pch.h"
#include "CBossBulletScript.h"

#include <Engine/CAnimator2D.h>

#include <Engine/CAnimation2D.h>

#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr float BULLET_SPEED = 400.f;
	constexpr float LIFE_TIME    = 3.f;
}

CBossBulletScript::CBossBulletScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSBULLETSCRIPT), DEFINE_NAME(CBossBulletScript)}
  , m_accTime{0}
  , m_isDead{false} {}

CBossBulletScript::~CBossBulletScript() = default;

void CBossBulletScript::update()
{
	if (true == m_isDead
	    && Animator2D()->GetCurrentAnim()->IsFinish())
	{
		GetOwner()->Destroy();
		return;
	}

	Vec3 position = Transform()->GetRelativePos();

	position.x += m_direction.x * DT * BULLET_SPEED;
	position.y += m_direction.y * DT * BULLET_SPEED;

	Transform()->SetRelativePos(position);

	m_accTime += DT;

	if (m_accTime > LIFE_TIME)
	{
		GetOwner()->Destroy();
	}
}

void CBossBulletScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() == PLAYER_LAYER)
	{
		Animator2D()->Play(L"ATTACK", false);
		m_isDead = true;
	}
}
