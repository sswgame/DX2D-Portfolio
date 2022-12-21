#include "pch.h"
#include "CSkelScript.h"

#include <Engine/CAnimator2D.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"


CSkelScript::CSkelScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELSCRIPT), DEFINE_NAME(CSkelScript)}
  , m_pWeapon{nullptr}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_pPlayer{nullptr}
  , m_searchRange{600.f}
  , m_attackRange{100.f}
  , m_speed{200.f}
  , m_attackDelay{0.25f}
  , m_accTime{0}
{
	m_keyPrefabSkelSword = L"prefab\\SKEL_SWORD.pref";
}

CSkelScript::~CSkelScript() {}

void CSkelScript::start()
{
	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabSkelSword);
	m_pWeapon            = pPrefab->Instantiate();
	m_pWeapon->SetLayerIndex(MONSTER_ATTACK_LAYER);
	CSceneMgr::GetInst()->AddChild(GetOwner(), m_pWeapon);

	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::SKELETON));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::SKELETON));
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"MOVE");
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::SKELETON);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CSkelScript::update()
{
	const Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3       skelPosition   = Transform()->GetRelativePos();

	const float distance = playerPosition.x - skelPosition.x;

	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			if (fabs(distance) <= m_searchRange)
			{
				m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
				m_pUnitScript->SetRepeat(true);
			}
		}
		break;
	case UNIT_STATE::MOVE:
		{
			if (fabs(distance) >= m_attackRange)
			{
				if (distance < 0.f)
				{
					skelPosition.x -= m_speed * DT;
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
				}
				else
				{
					skelPosition.x += m_speed * DT;
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
				}
				Transform()->SetRelativePos(skelPosition);
			}
			else
			{
				m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
			}
		}
		break;

	case UNIT_STATE::DEAD: break;

	case UNIT_STATE::ATTACK:
		{
			m_accTime += DT;
			if (m_accTime >= m_attackDelay)
			{
				m_accTime = 0.f;
				m_pWeapon->Animator2D()->Play(L"ATTACK", false);

				if (m_pWeapon->Animator2D()->GetCurrentAnim()->IsFinish())
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);

					m_pWeapon->Animator2D()->GetCurrentAnim()->Reset();
					m_pWeapon->Animator2D()->Play(L"IDLE", true);
				}
			}
			break;

		default: ;
		}
	}
}
