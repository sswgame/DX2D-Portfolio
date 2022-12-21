#include "pch.h"
#include "CSkelDogScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CMgrScript.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr float SKEL_DOG_JUMP_POWER = 500.f;
}

CSkelDogScript::CSkelDogScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELDOGSCRIPT), DEFINE_NAME(CSkelDogScript)}
  , m_pPlayer{nullptr}
  , m_pPlayerScript{nullptr}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_speed{300.f}
  , m_attackDelay{0.5f}
  , m_accTime{0}
  , m_attackRange{50.f}
  , m_searchRange{200.f}
  , m_isAttacking{false} {}

CSkelDogScript::~CSkelDogScript() = default;

void CSkelDogScript::start()
{
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::SKELETON_DOG));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::SKELETON_DOG));
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"MOVE");
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	m_pUnitScript->SetRepeat(true);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::SKELETON_DOG);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CSkelDogScript::update()
{
	const Vec3& playerPosition  = m_pPlayer->Transform()->GetRelativePos();
	Vec3        skelDogPosition = Transform()->GetRelativePos();

	const float distance = playerPosition.x - skelDogPosition.x;
	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			if (fabs(distance) <= m_searchRange)
		{
			m_pUnitScript->SetRepeat(false);
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				if (true == m_isAttacking)
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
					m_attackDirection = (distance < 0.f) ? Vec3{-1.f, 0.f, 0.f} : Vec3{1.f, 0.f, 0.f};
				}
				else
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
					m_pUnitScript->SetRepeat(true);
				}
			}
		}
		}
		break;
	case UNIT_STATE::MOVE:
		{
			if (fabs(distance) >= m_attackRange)
		{
			if (distance < 0.f)
			{
				skelDogPosition.x -= m_speed * DT;
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
			}
			else
			{
				skelDogPosition.x += m_speed * DT;
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
			}
			Transform()->SetRelativePos(skelDogPosition);
		}
			else
			{
				if (false == m_isAttacking)
				{
					m_isAttacking = true;
					m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
					m_pUnitScript->SetRepeat(false);
				}
			}
		}
		break;

	case UNIT_STATE::DEAD: break;

	case UNIT_STATE::ATTACK:
		{
			if (true == m_isAttacking)
		{
			m_isAttacking = false;
			m_pUnitScript->GetGravityScript()->SetVelocity(Vec2{0.f, SKEL_DOG_JUMP_POWER});
			m_pUnitScript->SetGround(false);
		}
			else if (false == m_pUnitScript->IsGround())
			{
				skelDogPosition.x += m_attackDirection.x * DT * m_speed;
				skelDogPosition.y += m_attackDirection.y * DT * m_speed;
				Transform()->SetRelativePos(skelDogPosition);
			}
			else if (m_pUnitScript->IsGround()
			         && false == m_isAttacking)
			{
				m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
				m_pUnitScript->SetRepeat(true);
				m_attackOnce = false;
			}
			break;
		default: ;
	}
	}
}

void CSkelDogScript::OnCollisionExit(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player" && false == m_attackOnce)
	{
		m_attackOnce           = true;
		CPlayerScript* pScript = static_cast<CPlayerScript*>(_OtherObject->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT));
		pScript->OnAttacked();
	}
}
