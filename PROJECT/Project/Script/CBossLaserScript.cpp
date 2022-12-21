#include "pch.h"
#include "CBossLaserScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "NO_SCRIPT/Define.h"

CBossLaserScript::CBossLaserScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSLASERSCRIPT), DEFINE_NAME(CBossLaserScript)}
  , m_pHead{nullptr}
  , m_pBody{nullptr}
  , m_isDead{false} {}

CBossLaserScript::~CBossLaserScript() = default;

void CBossLaserScript::start()
{
	FindChild();
	m_pHead->Animator2D()->Play(L"LASER_HEAD", false);
	m_pBody->Animator2D()->Play(L"LASER_BODY", false);
}

void CBossLaserScript::update()
{
	if (false == m_isDead)
	{
		if (m_pHead->Animator2D()->GetCurrentAnim()->IsFinish()
		    || m_pBody->Animator2D()->GetCurrentAnim()->IsFinish())
		{
			GetOwner()->Destroy();
			m_isDead = true;
		}
	}
}

void CBossLaserScript::FindChild()
{
	auto& vecChild = GetOwner()->GetChild();
	for (auto& pChild : vecChild)
	{
		if (L"LASER_HEAD" == pChild->GetName()) { m_pHead = pChild; }
		else if (L"LASER_BODY" == pChild->GetName()) { m_pBody = pChild; }
	}
}
