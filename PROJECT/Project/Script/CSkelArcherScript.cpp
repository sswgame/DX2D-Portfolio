#include "pch.h"
#include "CSkelArcherScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CMgrScript.h>

#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

CSkelArcherScript::CSkelArcherScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELARCHERSCRIPT), DEFINE_NAME(CSkelArcherScript)}
  , m_pMonsterScript{nullptr}
  , m_pUnitScript{nullptr}
  , m_pBow{nullptr}
  , m_pPlayer{nullptr}
  , m_accTime{0}
  , m_attackDelay{3.f}
  , m_attackRange{1000.f} {}

CSkelArcherScript::~CSkelArcherScript() = default;

void CSkelArcherScript::start()
{
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::SKELETON_ARCHER));
	m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::SKELETON_ARCHER));
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);

	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::SKELETON_ARCHER);

	m_pBow = GetOwner()->GetChild()[0];
	assert(m_pBow);
	m_pBow->SetLayerIndex(DEFAULT_LAYER);
	m_pBow->Animator2D()->Play(L"IDLE", true);

	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CSkelArcherScript::update()
{
	const Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3       skelPosition   = Transform()->GetRelativePos();

	const float distance = playerPosition.x - skelPosition.x;

	if (fabs(distance) < m_attackRange)
	{
		m_accTime += DT;
		if (m_accTime > m_attackDelay)
		{
			m_accTime = 0.f;
			m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
		}
		else
		{
			m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
		}
	}
	else
	{
		m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	}

	if (distance < 0.f)
	{
		m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
	}
	else
	{
		m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
	}
}
