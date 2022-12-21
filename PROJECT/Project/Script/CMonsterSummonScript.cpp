#include "pch.h"
#include "CMonsterSummonScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CMgrScript.h>
#include "CObjectManagerScript.h"

namespace
{
	inline const wchar_t* MONSTER_DIE   = L"sound\\MONSTER_DIE.wav";
	inline const wchar_t* SPAWN_MONSTER = L"sound\\SPAWN_MONSTER.wav";
}

CMonsterSummonScript::CMonsterSummonScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MONSTERSUMMONSCRIPT), DEFINE_NAME(CMonsterSummonScript)}
  , m_isDead{false}
{
	AddScriptParam("Monster", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefab);
}

CMonsterSummonScript::CMonsterSummonScript(const CMonsterSummonScript& other)
	:
	CScript{other}
  , m_keyPrefab{other.m_keyPrefab}
  , m_isDead{false}

{
	ClearScriptParam();
	AddScriptParam("Monster", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefab);
}

CMonsterSummonScript::~CMonsterSummonScript() = default;

void CMonsterSummonScript::SaveToScene(FILE* _pFile)
{
	SaveWStringToFile(m_keyPrefab, _pFile);
}

void CMonsterSummonScript::LoadFromScene(FILE* _pFile)
{
	LoadWStringFromFile(m_keyPrefab, _pFile);
}

void CMonsterSummonScript::start()
{
	if (false == m_isDead)
		Animator2D()->Play(L"CREATE", false);

	GetOwner()->SetLayerIndex(MONSTER_SPAWN_LAYER);
	CResMgr::GetInst()->FindRes<CSound>(SPAWN_MONSTER)->Play(1, 1.f);
}

void CMonsterSummonScript::update()
{
	if (false == m_isDead
	    && Animator2D()->GetCurrentAnim()->IsFinish())
	{
		Animator2D()->GetCurrentAnim()->Reset();
		CGameObject* pMonster = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefab)->Instantiate();
		pMonster->Transform()->SetRelativePos(Transform()->GetRelativePos() + Vec3{0.f, 0.f, 5.f});
		CSceneMgr::GetInst()->SpawnObject(pMonster, MONSTER_LAYER);
		GetOwner()->Destroy();
	}
	else if (true == m_isDead
	         && Animator2D()->GetCurrentAnim()->IsFinish())
	{
		Animator2D()->GetCurrentAnim()->Reset();
		GetOwner()->Destroy();
	}
}

void CMonsterSummonScript::SetDead()
{
	Animator2D()->Play(L"DESTROY", false);
	CResMgr::GetInst()->FindRes<CSound>(MONSTER_DIE)->Play(1, 1.f);
	m_isDead = true;
}
