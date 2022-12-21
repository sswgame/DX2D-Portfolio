#include "pch.h"
#include "CBansheeScript.h"

#include <Engine/CAnimator2D.h>

#include <Engine/CAnimation2D.h>
#include "CMonsterScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"

#include "CBansheeBulletScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	inline const wchar_t* BANSHEE_ATTACK = L"sound\\BANSHEE_ATTACK.wav";
}

CBansheeScript::CBansheeScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BANSHEESCRIPT), DEFINE_NAME(CBansheeScript)}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_accTime{0}
  , m_coolTime{3.f}
{
	m_keyBulletPrefab = L"prefab\\BANSHEE_BULLET.pref";
}

CBansheeScript::~CBansheeScript() = default;

void CBansheeScript::start()
{
	CMgrScript<CObjectManagerScript>::GetInst()->RegisterPrefab(m_keyBulletPrefab, 20);
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetHP(static_cast<int>(UNIT_HP::BANSHEE));
	m_pUnitScript->SetMaxHP(static_cast<int>(UNIT_HP::BANSHEE));

	m_pUnitScript->SetAnimation(UNIT_STATE::ATTACK, L"ATTACK");
	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, L"IDLE");
	m_pUnitScript->SetRepeat(true);
	m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
	m_pMonsterScript->SetMonsterType(MONSTER_TYPE::BANSHEE);
}

void CBansheeScript::update()
{
	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
		{
			m_accTime += DT;
			if (m_accTime > m_coolTime)
			{
				m_accTime = 0.f;
				CreateBullet();
				m_pUnitScript->SetUnitState(UNIT_STATE::ATTACK);
				m_pUnitScript->SetRepeat(false);
				CResMgr::GetInst()->FindRes<CSound>(BANSHEE_ATTACK)->Play(1, 1.f);
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
			}
		}

		break;
	default: ;
	}
}

void CBansheeScript::CreateBullet()
{
	for (int i = 0; i < 12; ++i)
	{
		CGameObject* pAttack = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyBulletPrefab);
		Vec3         pos     = Transform()->GetRelativePos();
		pos.z                = 5.f * i;
		pAttack->Transform()->SetRelativePos(pos);

		Vec3                  test          = pAttack->Transform()->GetRelativePos();
		CBansheeBulletScript* pBulletScript = static_cast<CBansheeBulletScript*>(pAttack->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::BANSHEEBULLETSCRIPT)));

		float radian = XMConvertToRadians(i * 30.f);
		pBulletScript->SetDirection(Vec2{cosf(radian), sinf(radian)});
		pBulletScript->ResetObject();

		CSceneMgr::GetInst()->SpawnObject(pAttack, MONSTER_ATTACK_LAYER);
	}
}
