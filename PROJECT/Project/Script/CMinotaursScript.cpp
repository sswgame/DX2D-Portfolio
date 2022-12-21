#include "pch.h"
#include "CMinotaursScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>
#include <Engine/CMgrScript.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

CMinotaursScript::CMinotaursScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MINOTAURSSCRIPT), DEFINE_NAME(CMinotaursScript)}
  , m_pMonsterScript{nullptr}
  , m_pUnitScript{nullptr}
  , m_pPlayer{nullptr}
  , m_pWeaponColliderObject{nullptr}
  , m_speed{600.f}
  , m_searchRange{700.f}
  , m_attackRange{200.f}
  , m_accTime{0}
  , m_attackDelay{2.f}
  , m_isSkillUsed{false} {}

CMinotaursScript::~CMinotaursScript() = default;

void CMinotaursScript::start()
{
	m_pWeaponColliderObject = GetOwner()->GetChild()[0];
	m_pWeaponColliderObject->SetLayerIndex(MONSTER_ATTACK_LAYER);
	m_pWeaponColliderObject->Deactivate();

	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::MINOTAURS));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::MINOTAURS));
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::SKILL, L"SKILL");
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	m_pUnitScript->SetRepeat(true);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::MINOTAURS);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CMinotaursScript::update()
{
	const Vec3& playerPosition  = m_pPlayer->Transform()->GetRelativePos();
	Vec3        monsterPosition = Transform()->GetRelativePos();

	const float distance = playerPosition.x - monsterPosition.x;

	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			if (fabs(distance) <= m_searchRange)
			{
				if (distance < 0.f)
				{
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
				}
				else
				{
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
				}

				m_accTime += DT;
				m_pUnitScript->SetRepeat(false);
				if (Animator2D()->GetCurrentAnim()->IsFinish())
				{
					Animator2D()->GetCurrentAnim()->Reset();


					if (m_accTime >= m_attackDelay)
					{
						if (fabs(distance) <= m_attackRange)
						{
							m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
							if (false == m_pWeaponColliderObject->IsActive())
							{
								m_pWeaponColliderObject->Activate();
							}
						}
						else
						{
							m_pUnitScript->SetUnitState(UNIT_STATE::SKILL);
							m_isSkillUsed = true;
						}
						m_pUnitScript->SetRepeat(false);
						m_attackDirection = (distance < 0.f) ? Vec3{-1.f, 0.f, 0.f} : Vec3{1.f, 0.f, 0.f};
					}
				}
			}
			else
			{
				m_pUnitScript->SetRepeat(true);
			}
		}
		break;

	case UNIT_STATE::SKILL:
		{
			if (false == Animator2D()->GetCurrentAnim()->IsFinish())
			{
				monsterPosition.x += m_attackDirection.x * m_speed * DT;
				monsterPosition.y += m_attackDirection.y * m_speed * DT;
				Transform()->SetRelativePos(monsterPosition);
			}
			else
			{
				Animator2D()->GetCurrentAnim()->Reset();
				m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
				m_pUnitScript->SetRepeat(false);
				m_isSkillUsed = false;
				m_accTime     = 0.f;
			}
		}
		break;
	case UNIT_STATE::DEAD: break;

	case UNIT_STATE::ATTACK:
		{
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				m_pWeaponColliderObject->Deactivate();
				m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
				m_pUnitScript->SetRepeat(true);
				m_accTime = 0.f;
			}
			break;
		default: ;
		}
	}
}

void CMinotaursScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (m_isSkillUsed && _OtherObject->GetLayerIndex() == PLAYER_LAYER)
	{
		CUnitScript* pScript = static_cast<CUnitScript*>(_OtherObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
		pScript->SetUnitState(UNIT_STATE::IDLE);

		const Vec3& monsterPosition      = Transform()->GetRelativePos();
		const Vec3& monsterColliderScale = Collider2D()->GetWorldScale();
		const Vec3& playerColliderScale  = _OtherObject->Collider2D()->GetWorldScale();
		if (UNIT_DIRECTION::LEFT == m_pUnitScript->GetUnitDirection())
		{
			_OtherObject->Transform()->SetRelativePos(monsterPosition - Vec3{monsterColliderScale.x * .5f, 0.f, 0.f} -
			                                          Vec3{
				                                          playerColliderScale.x * 0.5f
				                                        , 0.f
				                                        , 0.f
			                                          });
		}
		else
		{
			_OtherObject->Transform()->SetRelativePos(monsterPosition + Vec3{monsterColliderScale.x * .5f, 0.f, 0.f} +
			                                          Vec3{
				                                          playerColliderScale.x * 0.5f
				                                        , 0.f
				                                        , 0.f
			                                          });
		}
	}
}

void CMinotaursScript::OnCollision(CGameObject* _OtherObject)
{
	if (m_isSkillUsed && _OtherObject->GetLayerIndex() == PLAYER_LAYER)
	{
		Vec3 playerPosition = _OtherObject->Transform()->GetRelativePos();
		if (UNIT_DIRECTION::LEFT == m_pUnitScript->GetUnitDirection())
		{
			playerPosition.x -= m_speed * 1.05f * DT;
		}
		else
		{
			playerPosition.x += m_speed * 1.05f * DT;
		}
		_OtherObject->Transform()->SetRelativePos(playerPosition);
	}
}

void CMinotaursScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);
}
