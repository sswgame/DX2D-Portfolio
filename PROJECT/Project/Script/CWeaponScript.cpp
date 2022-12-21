#include "pch.h"
#include "CWeaponScript.h"

#include <Engine/CAnimator2D.h>

#include "CCameraScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"

namespace
{
	constexpr float SKILL_COOL_TIME = 3.f;
}

CWeaponScript::CWeaponScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::WEAPONSCRIPT), DEFINE_NAME(CWeaponScript)}
  , m_pAttackObject{nullptr}
  , m_pAttackScript{nullptr}
  , m_damage{0}
  , m_attackSpeed{0}
  , m_attackDelay{0}
  , m_skillCoolTime{0}
  , m_canAttack{false}
{
	AddScriptParam("DAMAGE", SCRIPTPARAM_TYPE::INT, &m_damage);
	AddScriptParam("ATTACK_DELAY", SCRIPTPARAM_TYPE::FLOAT, &m_attackDelay);
	AddScriptParam("ATTACK_SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_attackSpeed);
	AddScriptParam("SKILL_COOLTIME", SCRIPTPARAM_TYPE::FLOAT, &m_skillCoolTime);
	AddScriptParam("EFFECT PREFAB", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefabEffect);
	AddScriptParam("ICON PREFAB", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefabIcon);
	AddScriptParam("SOUND", SCRIPTPARAM_TYPE::SOUND, &m_keySound);
}

CWeaponScript::CWeaponScript(const CWeaponScript& other)
	:
	CScript{other}
  , m_keyPrefabEffect{other.m_keyPrefabEffect}
  , m_pAttackObject{other.m_pAttackObject}
  , m_pAttackScript{other.m_pAttackScript}
  , m_keyPrefabIcon{other.m_keyPrefabIcon}
  , m_keySound{other.m_keySound}
  , m_damage{other.m_damage}
  , m_attackSpeed{other.m_attackSpeed}
  , m_attackDelay{other.m_attackDelay}
  , m_skillCoolTime{other.m_skillCoolTime}
  , m_canAttack{false}
{
	ClearScriptParam();
	AddScriptParam("DAMAGE", SCRIPTPARAM_TYPE::INT, &m_damage);
	AddScriptParam("ATTACK_DELAY", SCRIPTPARAM_TYPE::FLOAT, &m_attackDelay);
	AddScriptParam("ATTACK_SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_attackSpeed);
	AddScriptParam("SKILL_COOLTIME", SCRIPTPARAM_TYPE::FLOAT, &m_skillCoolTime);
	AddScriptParam("EFFECT PREFAB", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefabEffect);
	AddScriptParam("ICON PREFAB", SCRIPTPARAM_TYPE::PREFAB, &m_keyPrefabIcon);
	AddScriptParam("SOUND", SCRIPTPARAM_TYPE::SOUND, &m_keySound);
}


CWeaponScript::~CWeaponScript() = default;

void CWeaponScript::start()
{
	GetOwner()->SetLayerIndex(DEFAULT_LAYER);
}

void CWeaponScript::update()
{
	if (nullptr != m_pAttackScript && m_pAttackScript->IsEnd())
	{
		m_pAttackObject = nullptr;
		m_pAttackScript = nullptr;
	}
}

void CWeaponScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_damage, sizeof(UINT), 1, _pFile);
	fwrite(&m_attackDelay, sizeof(float), 1, _pFile);
	fwrite(&m_attackSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_skillCoolTime, sizeof(float), 1, _pFile);
	SaveWStringToFile(m_keyPrefabEffect, _pFile);
	SaveWStringToFile(m_keyPrefabIcon, _pFile);
	SaveWStringToFile(m_keySound, _pFile);
}

void CWeaponScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_damage, sizeof(UINT), 1, _pFile);
	fread(&m_attackDelay, sizeof(float), 1, _pFile);
	fread(&m_attackSpeed, sizeof(float), 1, _pFile);
	fread(&m_skillCoolTime, sizeof(float), 1, _pFile);
	LoadWStringFromFile(m_keyPrefabEffect, _pFile);
	LoadWStringFromFile(m_keyPrefabIcon, _pFile);
	LoadWStringFromFile(m_keySound, _pFile);
}

void CWeaponScript::InstanciateEffect(const Vec3& position, const Vec3& rotation)
{
	CMgrScript<CObjectManagerScript>::GetInst()->RegisterPrefab(m_keyPrefabEffect);
	m_pAttackObject = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabEffect);
	m_pAttackObject->Transform()->SetRelativePos(position);
	m_pAttackObject->Transform()->SetRelativeRotation(rotation);
	m_pAttackObject->Animator2D()->Play(L"EFFECT", false);
	CSceneMgr::GetInst()->SpawnObject(m_pAttackObject, PLAYER_ATTACK_LAYER);

	m_pAttackScript = static_cast<CAttackScript*>(m_pAttackObject->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::ATTACKSCRIPT)));
	m_pAttackScript->SetColliderTime(m_attackSpeed);

	CResMgr::GetInst()->Load<CSound>(m_keySound, m_keySound)->Play(1, 1.f);
}
