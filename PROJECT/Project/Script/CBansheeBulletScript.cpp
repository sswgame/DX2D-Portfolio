#include "pch.h"
#include "CBansheeBulletScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

CBansheeBulletScript::CBansheeBulletScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BANSHEEBULLETSCRIPT), DEFINE_NAME(CBansheeBulletScript)}
  , m_destroyTime{2.f}
  , m_speed{350.f}
  , m_accTime{0}
  , m_state{BULLET_STATE::MOVE} {}

CBansheeBulletScript::~CBansheeBulletScript() {}

void CBansheeBulletScript::update()
{
	switch (m_state)
	{
	case BULLET_STATE::MOVE:
		{
			Vec3 position = Transform()->GetRelativePos();
			position.x += m_direction.x * m_speed * DT;
			position.y += m_direction.y * m_speed * DT;

			Transform()->SetRelativePos(position);

			m_accTime += DT;
			if (m_accTime > m_destroyTime)
			{
				Animator2D()->Play(L"ATTACK", false);
				m_state   = BULLET_STATE::ON_ATTACK;
				m_accTime = 0.f;
			}
		}

		break;
	case BULLET_STATE::ON_ATTACK:
		{
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Animator2D()->GetCurrentAnim()->Reset();
				CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
				CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
				m_state = BULLET_STATE::MOVE;
			}
		}
		break;
	default: ;
	}
}

void CBansheeBulletScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() != PLAYER_LAYER)
	{
		return;
	}

	Animator2D()->Play(L"ATTACK", false);
	m_state = BULLET_STATE::ON_ATTACK;
}

void CBansheeBulletScript::ResetObject()
{
	Animator2D()->Play(L"MOVE", true);
	m_state   = BULLET_STATE::MOVE;
	m_accTime = 0.f;
}
