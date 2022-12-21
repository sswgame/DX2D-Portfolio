#include "pch.h"
#include "CSmallBatScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CBatBulletScript.h"
#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"

namespace
{
	inline const wchar_t* SMALL_BAT_ATTACK = L"sound\\BAT_IDLE_2.wav";
}

CSmallBatScript::CSmallBatScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SMALLBATSCRIPT), DEFINE_NAME(CSmallBatScript)}
  , m_attackCoolTime{3.f}
  , m_moveCoolTime{2.f}
  , m_speed{50.f}
	//, m_pBullet{nullptr}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_pPlayer{nullptr}
  , m_accTime{0}
  , m_moveDirectionRandom{0}
  , m_type{BAT_TYPE::NORMAL}
{
	m_keyPrefabBullet = L"prefab\\BAT_BULLET.pref";
}

CSmallBatScript::~CSmallBatScript() {}

void CSmallBatScript::start()
{
	m_type        = static_cast<BAT_TYPE>(GetRandomInRange(0, 1));
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::SMALL_BAT));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::SMALL_BAT));

	if (BAT_TYPE::NORMAL == m_type)
	{
		m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
		m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"IDLE");
		m_pUnitScript->SetAnimation(UNIT_STATE::DEAD, L"DEAD");
	}
	else
	{
		m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE_RED");
		m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, L"IDLE_RED");
		m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK_RED");
		m_pUnitScript->SetAnimation(UNIT_STATE::DEAD, L"DEAD");
	}
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	m_pUnitScript->SetRepeat(true);

	CMgrScript<CObjectManagerScript>::GetInst()->RegisterPrefab(m_keyPrefabBullet);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::SMALL_BAT);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CSmallBatScript::update()
{
	Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3 batPosition    = Transform()->GetRelativePos();

	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			if (playerPosition.x - batPosition.x > 0.f)
			{
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
			}
			else
			{
				m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
			}

			m_accTime += DT;
			if (m_accTime > m_attackCoolTime)
			{
				m_accTime = 0.f;

				if (BAT_TYPE::RED == m_type)
				{
					m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
					m_pUnitScript->SetRepeat(false);
					CResMgr::GetInst()->FindRes<CSound>(SMALL_BAT_ATTACK)->Play(1, 1.f);
				}
				else
				{
					m_moveDirectionRandom = powf(-1, static_cast<float>(GetRandomInRange(0, 1)));
					m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
				}
			}
		}
		break;
	case UNIT_STATE::MOVE:
		{
			m_accTime += DT;
			if (m_accTime > m_moveCoolTime)
			{
				m_accTime = 0.f;
				m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
			}
			else
			{
				Vec3 directionVector = playerPosition - batPosition;
				directionVector.Normalize();
				batPosition += directionVector * m_speed * DT * m_moveDirectionRandom;
				if (playerPosition.x - batPosition.x > 0.f)
				{
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
				}
				else
				{
					m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
				}
				Transform()->SetRelativePos(batPosition);
			}
		}
		break;
	/*case UNIT_STATE::DEAD:
		{
			if (nullptr == m_pBullet) { return; }

			CBatBulletScript* pScript = static_cast<CBatBulletScript*>(m_pBullet->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
			if (pScript->IsMoving() == false)
			{
				CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(m_pBullet);
				CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(m_pBullet);
			}
			m_pBullet = nullptr;
		}
		break;*/
	case UNIT_STATE::ATTACK:
		{
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				CreateBullet();
				m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
				m_moveDirectionRandom = powf(-1, static_cast<float>(GetRandomInRange(0, 1)));
				m_pUnitScript->SetRepeat(true);
			}
		}
		break;
	default: ;
	}
}

void CSmallBatScript::CreateBullet()
{
	const Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3       batPosition    = Transform()->GetRelativePos();
	m_bulletDirection         = playerPosition - batPosition;
	m_bulletDirection.Normalize();

	batPosition.z += 5.f;

	CGameObject* pBullet = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabBullet);
	pBullet->Transform()->SetRelativePos(batPosition);
	CBatBulletScript* pScript = static_cast<CBatBulletScript*>(pBullet->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
	pScript->SetDirection(Vec2{m_bulletDirection.x, m_bulletDirection.y});
	pScript->ResetObject();
	CSceneMgr::GetInst()->SpawnObject(pBullet, MONSTER_ATTACK_LAYER);
	//m_pBullet = pBullet;
	pScript->Move();
}
