#include "pch.h"
#include "CGiantBatScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CBatBulletScript.h"
#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"

namespace
{
	inline const wchar_t* GIANT_BAT_ATTACK = L"sound\\BAT_IDLE_1.wav";
}

CGiantBatScript::CGiantBatScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::GIANTBATSCRIPT), DEFINE_NAME(CGiantBatScript)}
  , m_attackCoolTime{3.f}
  , m_maxBulletCount{20}
  , m_currentBulletCount{0}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_pPlayer{nullptr}
  , m_arrBullets{}
  , m_range{80.f}
  , m_accTime{0}
  , m_bulletTimer{0}
  , m_bulletCreationTerm{0.5f}
  , m_type{BAT_TYPE::NORMAL}
{
	m_keyPrefabBullet = L"prefab\\BAT_BULLET.pref";
}

CGiantBatScript::~CGiantBatScript() = default;

void CGiantBatScript::start()
{
	m_type        = static_cast<BAT_TYPE>(GetRandomInRange(0, 1));
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::GIANT_BAT));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::GIANT_BAT));

	if (BAT_TYPE::NORMAL == m_type)
	{
		m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
		m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
		m_pUnitScript->SetAnimation(UNIT_STATE::DEAD, L"DEAD");
		m_bulletCreationTerm = 0.3f;
	}
	else
	{
		m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE_RED");
		m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK_RED");
		m_pUnitScript->SetAnimation(UNIT_STATE::DEAD, L"DEAD");
		m_bulletCreationTerm = 0.03f;
	}
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	m_pUnitScript->SetRepeat(true);

	CMgrScript<CObjectManagerScript>::GetInst()->RegisterPrefab(m_keyPrefabBullet, 40);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::GIANT_BAT);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CGiantBatScript::update()
{
	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
			Vec3 batPosition    = Transform()->GetRelativePos();

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
				m_accTime         = 0.f;
				m_bulletDirection = playerPosition - batPosition;
				m_bulletDirection.Normalize();

				m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
				m_pUnitScript->SetRepeat(false);
		}
		}
		break;
	case UNIT_STATE::MOVE:
		break;
	case UNIT_STATE::DEAD:
		{
			if (BAT_TYPE::RED == m_type)
			{
				for (int i = 0; i < m_arrBullets.size(); ++i)
				{
					if (nullptr == m_arrBullets[i])
					{
						continue;
					}
					CBatBulletScript* pScript = static_cast<CBatBulletScript*>(m_arrBullets[i]->
						GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
					if (m_arrBullets[i]->IsInScene()/*BAT_BULLET_STATE::MOVE != pScript->GetState()*/
/*					    && false == pScript->IsReturned()*/)
					{
						/*	pScript->SetReturn(true);*/
						CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(m_arrBullets[i]);
						CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(m_arrBullets[i]);
					}
					m_arrBullets[i] = nullptr;
				}
				m_currentBulletCount = 0;
			}
		}
		break;
	case UNIT_STATE::ATTACK:
		{
			if (Animator2D()->GetCurrentAnim()->IsFinish())
		{
			Animator2D()->GetCurrentAnim()->Reset();
			m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
			m_pUnitScript->SetRepeat(true);

			if (BAT_TYPE::RED == m_type)
			{
				MoveBullet();
				m_currentBulletCount = 0;
			}

			CResMgr::GetInst()->FindRes<CSound>(GIANT_BAT_ATTACK)->Play(1, 1.f);
		}
			else
			{
				CreateBullet();
			}
	}
		break;
	default: ;
	}
}

void CGiantBatScript::CreateBullet()
{
	m_bulletTimer += DT;
	if (m_bulletTimer > m_bulletCreationTerm
	    && m_currentBulletCount < m_maxBulletCount)
	{
		if (BAT_TYPE::NORMAL == m_type)
		{
			CreateBulletNormal();
		}
		else
		{
			CreateBulletRed();
		}
	}
}

void CGiantBatScript::MoveBullet()
{
	for (int i = 0; i < m_arrBullets.size(); ++i)
	{
		if (nullptr == m_arrBullets[i])
		{
			continue;
		}
		CBatBulletScript* pScript = static_cast<CBatBulletScript*>(m_arrBullets[i]->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
		if (BAT_BULLET_STATE::ATTACK != pScript->GetState())
		{
			pScript->Move();
			m_arrBullets[i] = nullptr;
		}

	}
}

void CGiantBatScript::CreateBulletRed()
{
	float radian = XMConvertToRadians(static_cast<float>(m_currentBulletCount) * 360 / static_cast<float>(
		                                  m_maxBulletCount));
	Vec3 bulletPosition = Transform()->GetRelativePos();
	bulletPosition.x += cosf(radian) * m_range;
	bulletPosition.y += sinf(radian) * m_range;
	bulletPosition.z = 5.f + m_currentBulletCount;

	CGameObject* pBullet = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabBullet);
	pBullet->Transform()->SetRelativePos(bulletPosition);

	CBatBulletScript* pScript = static_cast<CBatBulletScript*>(pBullet->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
	pScript->SetDirection(Vec2{m_bulletDirection.x, m_bulletDirection.y});
	pScript->ResetObject();

	CSceneMgr::GetInst()->SpawnObject(pBullet, MONSTER_ATTACK_LAYER);
	m_arrBullets[m_currentBulletCount] = pBullet;
	m_bulletTimer                      = 0.f;
	++m_currentBulletCount;
}

void CGiantBatScript::CreateBulletNormal()
{
	Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3 batPosition    = Transform()->GetRelativePos();

	Vec3 directionVector = playerPosition - batPosition;
	directionVector.Normalize();


	static int temp[3] = {0, 1, -1};
	for (int i = 0; i < 3; ++i)
	{
		float radian        = XMConvertToRadians(30.f) * temp[i];
		m_bulletDirection.x = directionVector.x * cosf(radian) - directionVector.y * sinf(radian);
		m_bulletDirection.y = directionVector.x * sinf(radian) + directionVector.y * cosf(radian);

		CGameObject* pBullet = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabBullet);
		pBullet->Transform()->SetRelativePos(batPosition);
		CBatBulletScript* pScript = static_cast<CBatBulletScript*>(pBullet->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::BATBULLETSCRIPT)));
		pScript->SetDirection(Vec2{m_bulletDirection.x, m_bulletDirection.y});
		pScript->ResetObject();
		CSceneMgr::GetInst()->SpawnObject(pBullet, MONSTER_ATTACK_LAYER);
		m_bulletTimer = 0.f;
		pScript->Move();
		//++m_currentBulletCount;
	}
}
