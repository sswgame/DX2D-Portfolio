#include "pch.h"
#include "CAttackScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>
#include <Engine/CMgrScript.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

CAttackScript::CAttackScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::ATTACKSCRIPT), DEFINE_NAME(CAttackScript)}
  , m_isEnd{false}
  , m_attackAvailableTime{0}
  , m_accTime{0} {}

CAttackScript::~CAttackScript() = default;

void CAttackScript::update()
{
	m_accTime += DT;
	if (m_accTime >= m_attackAvailableTime)
	{
		m_accTime = 0.f;
		Collider2D()->Deactivate();
	}

	if (Animator2D()->GetCurrentAnim()->IsFinish())
	{
		m_isEnd   = true;
		m_accTime = 0.f;
		Collider2D()->Activate();
		Animator2D()->GetCurrentAnim()->Reset();
		CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
		CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
	}
}
