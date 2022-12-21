#include "pch.h"
#include "CMonsterScript.h"

#include <Engine/CAnimator2D.h>

#include <Engine/CAnimation2D.h>
#include <Engine/CCollider2D.h>

#include "CBossHPScript.h"
#include "CBossScript.h"
#include "CMonsterHPBarScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"

#include "CDamageUIScript.h"
#include "CMonsterSummonScript.h"
#include "CPlayerScript.h"
#include "CWeaponScript.h"

namespace
{
	constexpr float       MONSTER_ATTACKED_DELAY = 0.1f;
	inline const wchar_t* HIT_MONSTER    = L"sound\\HIT_MONSTER.wav";
}

CMonsterScript::CMonsterScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MONSTERSCRIPT), DEFINE_NAME(CMonsterScript)}
  , m_pUnitScript{nullptr}
  , m_pMonsterHPScript{nullptr}
  , m_pHPBar{nullptr}
  , m_monsterType{MONSTER_TYPE::NONE}
  , m_isAttacked{false}
  , m_accTime{0}
  , m_pPlayer{nullptr}
  , m_pWeaponScript{nullptr}
{
	m_collisionEffectKey = L"prefab\\COLLISION_EFFECT.pref";
	m_damageUIKey        = L"prefab\\DAMAGE_FONT.pref";
	m_keyPrefabHPBar     = L"prefab\\MONSTER_HP_BAR.pref";
	m_keyPrefabSummon    = L"prefab\\MONSTER_SUMMON.pref";
}

CMonsterScript::~CMonsterScript() = default;

void CMonsterScript::start()
{
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));

	if (L"BELLIAL" != GetOwner()->GetName())
	{
		m_pHPBar = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabHPBar)->Instantiate();
		m_pHPBar->Transform()->SetRelativePos(Vec3{0.f, -25.f, 0.f});
		m_pMonsterHPScript = static_cast<CMonsterHPBarScript*>(m_pHPBar->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERHPBARSCRIPT)));
		CSceneMgr::GetInst()->AddChild(GetOwner(), m_pHPBar);
	}


	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
}

void CMonsterScript::update()
{
	if (UNIT_STATE::DEAD == m_pUnitScript->GetUnitState())
	{
		CGameObject* pSummon = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabSummon)->Instantiate();
		pSummon->Transform()->SetRelativePos(Transform()->GetRelativePos());
		CMonsterSummonScript* pScript = static_cast<CMonsterSummonScript*>(pSummon->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSUMMONSCRIPT)));
		pScript->SetDead();
		CSceneMgr::GetInst()->SpawnObject(pSummon, DEFAULT_LAYER);
		GetOwner()->Destroy();
	}
	if (MONSTER_TYPE::BOSS != m_monsterType)
	{
		if (UNIT_DIRECTION::RIGHT == m_pUnitScript->GetUnitDirection())
		{
			m_pMonsterHPScript->SetFacingLeft(false);
		}
		else
		{
			m_pMonsterHPScript->SetFacingLeft(true);
		}
	}
	UpdateData();
}

void CMonsterScript::UpdateData()
{
	if (true == m_isAttacked)
	{
		m_accTime += DT;
		if (m_accTime > MONSTER_ATTACKED_DELAY)
		{
			m_isAttacked = false;
			m_accTime    = 0.f;
		}
	}
	int isAttacked = m_isAttacked;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, &isAttacked);
}

void CMonsterScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() == PLAYER_ATTACK_LAYER)
	{
		static CPlayerScript* pPlayerScript = static_cast<CPlayerScript*>(
			m_pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));

		CGameObject*              pHand       = pPlayerScript->GetHand();
		static CItemHolderScript* pItemHolder = static_cast<CItemHolderScript*>(
			pHand->GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));

		const CWeaponScript* pWeaponScript = pItemHolder->GetWeapon();
		assert(pWeaponScript);
		const UINT damage = pWeaponScript->GetDamage();
		m_pUnitScript->UpdateHP(damage);

		// hp bar
		if (MONSTER_TYPE::BOSS != m_monsterType)
		{
			m_pMonsterHPScript->SetHPRatio(static_cast<float>(GetTotalHP())
			                             , static_cast<float>(m_pUnitScript->GetHP()));
		}
		else
		{
			CBossScript* pScript = static_cast<CBossScript*>(GetOwner()->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSSCRIPT)));
				const float hpRatio
					= static_cast<float>(m_pUnitScript->GetHP()) / static_cast<float>(m_pUnitScript->GetMaxHP());
			pScript->GetHPScript()->setRemainHP(hpRatio);
			
		}

		m_isAttacked = true;
		CResMgr::GetInst()->FindRes<CSound>(HIT_MONSTER)->Play(1, 1.f);

		// damage UI
		CGameObject* pDamageUI = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_damageUIKey);
		Vec3         position  = Collider2D()->GetWorldPos() + Vec2{0.f, Collider2D()->GetWorldScale().y};
		position.z -= 1.f;
		pDamageUI->Transform()->SetRelativePos(position);
		CSceneMgr::GetInst()->SpawnObject(pDamageUI, DEFAULT_LAYER);

		CDamageUIScript* pScript =
			static_cast<CDamageUIScript*>(pDamageUI->GetScript(static_cast<UINT>(SCRIPT_TYPE::DAMAGEUISCRIPT)));
		pScript->ShowDamage(damage);
	}
}

UINT CMonsterScript::GetTotalHP()
{
	switch (m_monsterType)
	{
	case MONSTER_TYPE::SMALL_BAT: return static_cast<UINT>(UNIT_HP::SMALL_BAT);
	case MONSTER_TYPE::GIANT_BAT: return static_cast<UINT>(UNIT_HP::GIANT_BAT);
	case MONSTER_TYPE::SKELETON: return static_cast<UINT>(UNIT_HP::SKELETON);
	case MONSTER_TYPE::SKELETON_ARCHER: return static_cast<UINT>(UNIT_HP::SKELETON_ARCHER);
	case MONSTER_TYPE::SKELETON_DOG: return static_cast<UINT>(UNIT_HP::SKELETON_DOG);
	case MONSTER_TYPE::SKELETON_KNIGHT: return static_cast<UINT>(UNIT_HP::SKELETON_KNIGHT);
	case MONSTER_TYPE::BANSHEE: return static_cast<UINT>(UNIT_HP::BANSHEE);
	case MONSTER_TYPE::GHOST: return static_cast<UINT>(UNIT_HP::GHOST);
	case MONSTER_TYPE::MINOTAURS: return static_cast<UINT>(UNIT_HP::MINOTAURS);
	case MONSTER_TYPE::BOSS: return static_cast<UINT>(UNIT_HP::BOSS);


	case MONSTER_TYPE::NONE: break;
	case MONSTER_TYPE::END: break;
	default:
		break;
	}

	assert(nullptr);
	return 0;
}
