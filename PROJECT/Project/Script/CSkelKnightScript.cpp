#include "pch.h"
#include "CSkelKnightScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CMgrScript.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"


CSkelKnightScript::CSkelKnightScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELKNIGHTSCRIPT), DEFINE_NAME(CSkelKnightScript)}
  , m_pMonsterScript{nullptr}
  , m_pUnitScript{nullptr}
  , m_pPlayer{nullptr}
  , m_pWeaponColliderObject{nullptr}
  , m_attackRange{50.f}
  , m_accTime{0}
  , m_speed{300.f}
  , m_isAttacking{false} {}

CSkelKnightScript::~CSkelKnightScript() = default;

void CSkelKnightScript::start()
{
	m_pWeaponColliderObject = GetOwner()->GetChild()[0];
	m_pWeaponColliderObject->SetLayerIndex(MONSTER_ATTACK_LAYER);
	m_pWeaponColliderObject->Deactivate();

	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::SKELETON_KNIGHT));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::SKELETON_KNIGHT));
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"MOVE");
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::SKELETON_KNIGHT);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CSkelKnightScript::update()
{
	const Vec3& playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3        knightPosition = Transform()->GetRelativePos();

	const float distance = playerPosition.x - knightPosition.x;

	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			m_pUnitScript->SetRepeat(false);
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				if (true == m_isAttacking)
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
					m_pUnitScript->SetRepeat(false);
					m_attackDirection = (distance < 0.f) ? Vec3{-1.f, 0.f, 0.f} : Vec3{1.f, 0.f, 0.f};
					if (false == m_pWeaponColliderObject->IsActive())
					{
						m_pWeaponColliderObject->Activate();
					}
				}
				else
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
					m_pUnitScript->SetRepeat(true);
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
					knightPosition.x -= m_speed * DT;
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
				}
				else
				{
					knightPosition.x += m_speed * DT;
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
				}
				Transform()->SetRelativePos(knightPosition);
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
				/*m_isAttacking = false;*/
				if (Animator2D()->GetCurrentAnim()->IsFinish())
				{
					Animator2D()->GetCurrentAnim()->Reset();
					m_pWeaponColliderObject->Deactivate();
					m_isAttacking = false;
				}
			}
			else
			{
				m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
				m_pUnitScript->SetRepeat(true);
			}
			break;
		default: ;
		}
	}
}
