#include "pch.h"
#include "CGhostScript.h"

#include <Engine/CMgrScript.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

CGhostScript::CGhostScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::GHOSTSCRIPT), DEFINE_NAME(CGhostScript)}
  , m_pPlayer{nullptr}
  , m_pPlayerScript{nullptr}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_speed{300.f}
  , m_attackDelay{0.5f}
  , m_accTime{0}
  , m_attackTime{1.f}
  , m_attackRange{200.f} {}

CGhostScript::~CGhostScript() {}

void CGhostScript::start()
{
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::GHOST));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::GHOST));
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"MOVE");
	m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
	m_pUnitScript->SetRepeat(true);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::GHOST);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CGhostScript::update()
{
	const Vec3& playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3        ghostPosition  = Transform()->GetRelativePos();

	Vec3 directionVector = playerPosition - ghostPosition;


	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::MOVE:
		{
			if (playerPosition.x - ghostPosition.x > 0.f)
			{
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
				ghostPosition.z = 5.f;
			}
			else
			{
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
				ghostPosition.z = -5.f;
			}

			if (Vec2{directionVector.x, directionVector.y}.Length() > m_attackRange)
			{
				directionVector.Normalize();
				ghostPosition.x += directionVector.x * m_speed * DT;
				ghostPosition.y += directionVector.y * m_speed * DT;
				Transform()->SetRelativePos(ghostPosition);
			}
			else
			{
				m_accTime += DT;
				if (m_accTime > m_attackDelay)
				{
					directionVector.Normalize();
					m_accTime = 0.f;
					m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
					m_pUnitScript->SetRepeat(true);
					m_attackDirection = directionVector;
				}
			}
		}
		break;
	case UNIT_STATE::ATTACK:
		{
			m_accTime += DT;
			if (m_accTime < m_attackTime)
			{
				ghostPosition += m_attackDirection * DT * m_speed * 2.f;
				Transform()->SetRelativePos(ghostPosition);
			}
			else
			{
				m_accTime = 0.f;
				m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
				m_pUnitScript->SetRepeat(true);
			}
		}

		break;
	default: ;
	}
}

void CGhostScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player")
	{
		CPlayerScript* pScript = static_cast<CPlayerScript*>(_OtherObject->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT));
		pScript->OnAttacked();
	}
}

void CGhostScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);
}
