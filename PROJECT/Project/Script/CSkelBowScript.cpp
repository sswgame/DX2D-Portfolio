#include "pch.h"
#include "CSkelBowScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>

#include "CObjectManagerScript.h"
#include "CSkelArrowScript.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	inline const wchar_t* BOW_ANIMATION = L"sound\\BOW_ANIMATION.wav";
	inline const wchar_t* BOW_ATTACK    = L"sound\\BOW_ATTACK.wav";
}

CSkelBowScript::CSkelBowScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELBOWSCRIPT), DEFINE_NAME(CSkelBowScript)}
  , m_pPlayer{nullptr}
  , m_pSkelUnitScript{nullptr}
{
	m_keyPrefabArrow = L"prefab\\ARROW.pref";
}

CSkelBowScript::~CSkelBowScript() = default;

void CSkelBowScript::start()
{
	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
	if (GetOwner()->GetParent()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)))
	{
		m_pSkelUnitScript
			= static_cast<CUnitScript*>(GetOwner()->GetParent()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	}
	CMgrScript<CObjectManagerScript>::GetInst()->RegisterPrefab(m_keyPrefabArrow);

	Animator2D()->Play(L"IDLE", true);
}

void CSkelBowScript::update()
{
	//플레이어 위치는 하단 중앙이므로, 회전의 중심이 중앙쪽에서 시작되게 하려면 충돌체로 잡는 것이 좋다.
	const Vec3& playerPosition  = m_pPlayer->Collider2D()->GetWorldPos();
	Vec3        skelPosition    = GetOwner()->GetParent()->Collider2D()->GetWorldPos();
	const Vec3& skelRotation    = GetOwner()->GetParent()->Transform()->GetRelativeRotation();
	Vec3        directionVector = playerPosition - skelPosition;
	directionVector.Normalize();

	float rotationZ = acosf(directionVector.x);
	//아래쪽이라면 방향을 반대로 해야하므로 각도를 변경해야 한다.
	if (playerPosition.y - skelPosition.y < 0.f)
	{
		rotationZ *= -1;
	}

	// 방향 설정
	const Vec3& bowPosition = Transform()->GetRelativePos();
	if (UNIT_DIRECTION::RIGHT == m_pSkelUnitScript->GetUnitDirection())
	{
		Transform()->SetRelativePos(Vec3{bowPosition.x, bowPosition.y, -5.f});
		Transform()->SetRelativeRotation(Vec3(0.f, skelRotation.y, rotationZ));
	}
	else
	{
		Transform()->SetRelativePos(Vec3{bowPosition.x, bowPosition.y, 5.f});
		Transform()->SetRelativeRotation(Vec3(0.f, skelRotation.y, -rotationZ));
	}


	// 화살 생성
	if (UNIT_STATE::ATTACK == m_pSkelUnitScript->GetUnitState())
	{
		if (L"ATTACK" != Animator2D()->GetCurrentAnim()->GetName())
		{
			Animator2D()->Play(L"ATTACK", false);

			CResMgr::GetInst()->FindRes<CSound>(BOW_ATTACK)->Play(1, 1.f);

			CGameObject* pArrow = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabArrow);
			skelPosition.z -= 2.f;
			pArrow->Transform()->SetRelativePos(skelPosition);
			CSceneMgr::GetInst()->SpawnObject(pArrow, MONSTER_ATTACK_LAYER);

			CSkelArrowScript* pArrowScript
				= static_cast<CSkelArrowScript*>(pArrow->GetScript(static_cast<UINT>(SCRIPT_TYPE::SKELARROWSCRIPT)));

			const bool isLeft = (UNIT_DIRECTION::LEFT == m_pSkelUnitScript->GetUnitDirection());
			pArrowScript->SetDirection(Vec2{directionVector.x, directionVector.y}, rotationZ, isLeft);
			pArrowScript->ResetObject();
		}

		{
			if (L"ATTACK" == Animator2D()->GetCurrentAnim()->GetName()
			    && Animator2D()->GetCurrentAnim()->IsFinish())
			{
				CResMgr::GetInst()->FindRes<CSound>(BOW_ANIMATION)->Play(1, 1.f);
				Animator2D()->GetCurrentAnim()->Reset();
				Animator2D()->Play(L"IDLE", true);
			}
		}
	}
}
