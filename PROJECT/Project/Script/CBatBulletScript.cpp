#include "pch.h"
#include "CBatBulletScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	inline const wchar_t* BULLET_EXPLOSION = L"sound\\BULLET_EXPLOSION.wav";
}

CBatBulletScript::CBatBulletScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BATBULLETSCRIPT), DEFINE_NAME(CBatBulletScript)}
  , m_accTime{0}
  , m_speed{400.f}
  , m_state{BAT_BULLET_STATE::STOP}
  , m_isAttack{false} {}

CBatBulletScript::~CBatBulletScript() = default;

void CBatBulletScript::update()
{
	switch (m_state)
	{
	case BAT_BULLET_STATE::STOP:
		break;
	case BAT_BULLET_STATE::MOVE:
		{
			Vec3 position = Transform()->GetRelativePos();
			position.x += m_bulletDirection.x * m_speed * DT;
			position.y += m_bulletDirection.y * m_speed * DT;

			Transform()->SetRelativePos(position);
		}
		break;
	case BAT_BULLET_STATE::ATTACK:
		if (Animator2D()->GetCurrentAnim()->IsFinish())
		{
			GetOwner()->SetInScene(false);
			Animator2D()->GetCurrentAnim()->Reset();
			m_state = BAT_BULLET_STATE::STOP;
			CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
			CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
			//m_isReturned = true;
		}
		break;
	default: ;
	}
}

void CBatBulletScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (false == m_isAttack)
	{
		m_isAttack = true;
		Animator2D()->Play(L"ATTACK", false);
		Animator2D()->GetCurrentAnim()->Reset();
		m_state = BAT_BULLET_STATE::ATTACK;
		CResMgr::GetInst()->FindRes<CSound>(BULLET_EXPLOSION)->Play(1, 1.f);
	}
}

void CBatBulletScript::Move()
{
	//if (false == m_isReturned)
	{
		m_state = BAT_BULLET_STATE::MOVE;
	}
}

void CBatBulletScript::ResetObject()
{
	Animator2D()->GetCurrentAnim()->Reset();
	Animator2D()->Play(L"MOVE", true);
	m_state      = BAT_BULLET_STATE::STOP;
	m_isAttack   = false;
	//m_isReturned = false;
}
