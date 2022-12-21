#include "pch.h"
#include "CBossScript.h"

#include <Engine/CAnimator2D.h>

#include "CBossBulletScript.h"
#include "CBossHandScript.h"
#include "CMonsterScript.h"
#include "CUnitScript.h"

namespace
{
	constexpr float BOSS_PATTERN_DELAY    = 2.f;
	constexpr int   BULLET_LIMIT_COUNT    = 80;
	constexpr int   SWORD_LIMIT_COUNT     = 6;
	constexpr float BULLET_CREATION_DELAY = 0.1f;
	constexpr float SWORD_CREATION_DELAY  = 0.3f;
	constexpr float LASER_DELAY           = 2.f;
	constexpr int   FOUR_SIDE             = 4;
	constexpr float ROTATION_STEP         = 9.f;

//BOSS
	inline const wchar_t* BOSS_INTRO       = L"sound\\INTRO.wav";
	inline const wchar_t* DUNGEON_BOSS_BGM = L"sound\\DUNGEON_BOSS.wav";
	inline const wchar_t* BOSS_BULLET      = L"sound\\BOSS_BULLET.wav";
	inline const wchar_t* PREV_BGM         = L"sound\\DUNGEON_FIELD.wav";
}

CBossScript::CBossScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSSCRIPT), DEFINE_NAME(CBossScript)}
  , m_pattern{BOSS_PATTERN::RANDOM_GENERATION}
  , m_bulletCount{0}
  , m_swordCount{0}
  , m_pLeftHand{nullptr}
  , m_pRightHand{nullptr}
  , m_pDeadParticle{nullptr}
  , m_pHPBar{nullptr}
  , m_accTime{0}
  , m_pUnitScript{nullptr}
  , m_pMonsterScript{nullptr}
  , m_pHPBarScript{nullptr}
{
	m_keyPrefabBullet = L"prefab\\BOSS_BULLET.pref";
	m_keyPrefabSword  = L"prefab\\BOSS_SWORD.pref";
}

CBossScript::~CBossScript() = default;

void CBossScript::start()
{
	{
		m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
		m_pUnitScript->SetMaxHP(static_cast<UINT>(UNIT_HP::BOSS));
		m_pUnitScript->SetHP(static_cast<UINT>(UNIT_HP::BOSS));
	}
	{
		m_pMonsterScript = static_cast<CMonsterScript*>(GetOwner()->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
		m_pMonsterScript->SetMonsterType(MONSTER_TYPE::BOSS);
	}
	FindAllChild();
	Animator2D()->Play(L"IDLE", true);

	CResMgr::GetInst()->FindRes<CSound>(BOSS_INTRO)->Play(1, 1.f);
	CResMgr::GetInst()->FindRes<CSound>(PREV_BGM)->Stop();
	CResMgr::GetInst()->FindRes<CSound>(DUNGEON_BOSS_BGM)->Play(50, 1.f);

}

void CBossScript::update()
{
	switch (m_pattern)
	{
	case BOSS_PATTERN::RANDOM_GENERATION:
		m_accTime += DT;
		if (m_accTime > BOSS_PATTERN_DELAY)
		{
			m_pattern = static_cast<BOSS_PATTERN>(GetRandomInRange(0, 3));
			m_accTime = 0.f;
		}
		break;
	case BOSS_PATTERN::BULLET:
		if (0 == m_bulletCount)
		{
			if (L"ATTACK_BULLET" != Animator2D()->GetCurrentAnim()->GetName())
				Animator2D()->Play(L"ATTACK_BULLET", false);
		}

		if (BULLET_LIMIT_COUNT - 1 <= m_bulletCount)
		{
			m_pattern = BOSS_PATTERN::RANDOM_GENERATION;
			Animator2D()->Play(L"IDLE", true);
			m_accTime     = 0.f;
			m_bulletCount = 0;
		}
		else
		{
			m_accTime += DT;
			if (m_accTime > BULLET_CREATION_DELAY)
			{
				CResMgr::GetInst()->FindRes<CSound>(BOSS_BULLET)->Play(1, 1.f, true);
				for (int i = 0; i < FOUR_SIDE; ++i)
				{
					CreateBullet(i);
					++m_bulletCount;
				}
				m_accTime = 0.f;
			}
		}
		break;
	case BOSS_PATTERN::LASER:

		if (true == m_queueHandPattern.empty())
		{
			static CGameObject* arrHandsPattern[2][3] =
			{
				{m_pLeftHand, m_pRightHand, m_pLeftHand}
			  , {m_pRightHand, m_pLeftHand, m_pRightHand}
			};
			const int handPatternIndex = GetRandomInRange(0, 1);

			m_accTime = BOSS_PATTERN_DELAY + 0.1f;
			for (int i = 0; i < std::size(arrHandsPattern[handPatternIndex]); ++i)
			{
				m_queueHandPattern.push_back(arrHandsPattern[handPatternIndex][i]);
			}
		}

		m_accTime += DT;
		if (m_accTime > LASER_DELAY)
		{
			CGameObject* pHandPattern = m_queueHandPattern.front();
			m_queueHandPattern.pop_front();
			CBossHandScript* pScript
				= static_cast<CBossHandScript*>(
					pHandPattern->GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSHANDSCRIPT)));
			pScript->Attack();
			m_accTime = 0.f;
		}

		if (m_queueHandPattern.empty())
		{
			// 패턴 종료
			m_pattern = BOSS_PATTERN::RANDOM_GENERATION;
			m_accTime = 0.f;
		}
		break;
	case BOSS_PATTERN::SWORD:
		// 패턴 끝일 경우
		if (m_swordCount >= SWORD_LIMIT_COUNT)
		{
			m_pattern    = BOSS_PATTERN::RANDOM_GENERATION;
			m_swordCount = 0;
			m_accTime    = 0.f;
		}
		else
		{
			// 0.1초마다 소드 생성
			m_accTime += DT;
			if (m_accTime > SWORD_CREATION_DELAY)
			{
				CreateSword();
				++m_swordCount;
				m_accTime = 0.f;
			}
		}
		break;

	case BOSS_PATTERN::END: break;
	default: ;
	}
}

void CBossScript::CreateBullet(UINT index)
{
	Ptr<CPrefab> pPrefab  = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabBullet);
	CGameObject* pBullet  = pPrefab->Instantiate();
	Vec3         position = Transform()->GetRelativePos();
	position.x += 20.f;
	position.y -= 50.f;
	--position.z;
	position.z -= (m_bulletCount);
	pBullet->Transform()->SetRelativePos(position);

	CBossBulletScript* pBulletScript
		= static_cast<CBossBulletScript*>(pBullet->GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSBULLETSCRIPT)));
	const float cosine = cosf((ROTATION_STEP * (m_bulletCount / FOUR_SIDE) + index * XMConvertToRadians(90.f)));
	const float sine   = sinf((ROTATION_STEP * (m_bulletCount / FOUR_SIDE) + index * XMConvertToRadians(90.f)));
	pBulletScript->SetDirection(Vec2{cosine, sine});
	CSceneMgr::GetInst()->SpawnObject(pBullet, MONSTER_ATTACK_LAYER);
}

void CBossScript::CreateSword()
{
	constexpr Vec2  SWORD_STARTING_POINT = Vec2{375.f, 150.f};
	constexpr float SWORD_STEP           = 150.f;

	Vec3 position = Transform()->GetRelativePos();
	position.x -= SWORD_STARTING_POINT.x;
	position.x += m_swordCount * SWORD_STEP;
	position.y += SWORD_STARTING_POINT.y;
	position.z -= (m_swordCount + 1);

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabSword);
	CGameObject* pSword  = pPrefab->Instantiate();
	pSword->Transform()->SetRelativePos(position);
	pSword->Transform()->SetRelativeRotation(Vec3{0.f, 0.f, -XMConvertToRadians(90.f)});

	CSceneMgr::GetInst()->SpawnObject(pSword, MONSTER_ATTACK_LAYER);
}

void CBossScript::FindAllChild()
{
	CBossHandScript* pHandScript{};

	for (auto& pGameObject : GetOwner()->GetChild())
	{
		if (L"BOSS_LEFT_HAND" == pGameObject->GetName())
		{
			m_pLeftHand = pGameObject;
			pHandScript = static_cast<CBossHandScript*>(m_pLeftHand->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSHANDSCRIPT)));
			pHandScript->SetHandType(BOSS_HAND_TYPE::LEFT);
		}
		else if (L"BOSS_RIGHT_HAND" == pGameObject->GetName())
		{
			m_pRightHand = pGameObject;
			pHandScript  = static_cast<CBossHandScript*>(m_pRightHand->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSHANDSCRIPT)));
			pHandScript->SetHandType(BOSS_HAND_TYPE::RIGHT);
		}
		else if (L"BOSS_HP_BAR" == pGameObject->GetName())
		{
			m_pHPBar       = pGameObject;
			m_pHPBarScript = static_cast<CBossHPScript*>(m_pHPBar->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::BOSSHPSCRIPT)));
		}
		else
		{
			m_pDeadParticle = pGameObject;
			pGameObject->Deactivate();
		}
	}
}
