#include "pch.h"
#include "CBossHandScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CBossLaserScript.h"
#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr float HAND_SPEED        = 2500.f;
	constexpr float ATTACK_DELAY      = 0.5f;
	constexpr int   LASER_ON_FRAME    = 9;
	constexpr float HAND_ATTACK_RANGE = 5.f;

	inline const wchar_t* LASER = L"sound\\LASER.wav";
}

CBossHandScript::CBossHandScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSHANDSCRIPT), DEFINE_NAME(CBossHandScript)}
  , m_handType{BOSS_HAND_TYPE::LEFT}
  , m_state{}
  , m_accTime{0}
  , m_attackRange{HAND_ATTACK_RANGE}
  , m_isOnLaser{false}
  , m_pPlayer{nullptr}
{
	m_keyPrefabLaser = L"prefab\\BOSS_LASER.pref";
}

CBossHandScript::~CBossHandScript() = default;

void CBossHandScript::start()
{
	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
	Animator2D()->Play(L"IDLE", true);
}

void CBossHandScript::update()
{
	switch (m_state)
	{
	case BOSS_HAND_STATE::NONE:
		break;
	case BOSS_HAND_STATE::IDLE:
		m_accTime += DT;
		if (m_accTime > ATTACK_DELAY)
		{
			m_accTime = 0.f;
			m_state   = BOSS_HAND_STATE::CHASE;
		}
		break;
	case BOSS_HAND_STATE::CHASE:
		{
			const Vec3& playerPosition  = m_pPlayer->Transform()->GetRelativePos();
			Vec3        position        = Transform()->GetRelativePos();
			Vec3        directionVector = playerPosition - position;
			directionVector.Normalize();

			position.y += directionVector.y * DT * HAND_SPEED;
			static float temp{};
			temp += DT;
			Transform()->SetRelativePos(position);

			if ((fabs(playerPosition.y - position.y) <= m_attackRange
			     && L"ATTACK" != Animator2D()->GetCurrentAnim()->GetName()) || temp >= 1.f)
			{
				temp = 0.f;
				Animator2D()->Play(L"ATTACK", false);
				m_state = BOSS_HAND_STATE::ATTACK;
			}
		}
		break;
	case BOSS_HAND_STATE::ATTACK:
		{
			//레이저 공격
			if (false == m_isOnLaser
			    && Animator2D()->GetCurrentAnim()->GetCurrentFrameIndex() == LASER_ON_FRAME)
			{
				CResMgr::GetInst()->Load<CSound>(LASER, LASER)->Play(1, 1.f, true);

				Ptr<CPrefab> pPrefabLaser = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabLaser);
				CGameObject* pLaser = pPrefabLaser->Instantiate();

				Vec3         laserPosition = pLaser->Transform()->GetRelativePos();
				const Vec3&  handPosition = Transform()->GetRelativePos();
				laserPosition.z = (BOSS_HAND_TYPE::LEFT == m_handType) ? handPosition.z - 1.f : handPosition.z + 1.f;
				pLaser->Transform()->SetRelativePos(laserPosition);

				CSceneMgr::GetInst()->SpawnObject(pLaser, MONSTER_ATTACK_LAYER);
				CSceneMgr::GetInst()->AddChild(GetOwner(), pLaser);

				m_isOnLaser = true;
			}

			if (m_isOnLaser
			    && Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				Animator2D()->Play(L"IDLE", true);
				m_state     = BOSS_HAND_STATE::NONE;
				m_isOnLaser = false;
			}
		}
		break;
	default: ;
	}
}
