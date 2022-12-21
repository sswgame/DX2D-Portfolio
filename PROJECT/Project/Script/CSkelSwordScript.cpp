#include "pch.h"
#include "CSkelSwordScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>

#include "CObjectManagerScript.h"
#include "CPlayerScript.h"

namespace
{
	inline const wchar_t* SKEL_SWORD_ATTACK = L"sound\\SKEL_SWORD_ATTACK_1.wav";
}

CSkelSwordScript::CSkelSwordScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELSWORDSCRIPT), DEFINE_NAME(CSkelSwordScript)} {}

CSkelSwordScript::~CSkelSwordScript() = default;

void CSkelSwordScript::start()
{
	Animator2D()->Play(L"IDLE", true);
}

void CSkelSwordScript::update()
{
	const CAnimation2D* pCurrentAnimation = Animator2D()->GetCurrentAnim();
	const int           animationIndex    = pCurrentAnimation->GetCurrentFrameIndex();
	if (5 < animationIndex)
	{
		if (false == Collider2D()->IsActive())
		{
			Collider2D()->Activate();
			CResMgr::GetInst()->Load<CSound>(SKEL_SWORD_ATTACK, SKEL_SWORD_ATTACK)->Play(1, 1.f);
		}
	}
	if (animationIndex < 5)
	{
		if (Collider2D()->IsActive())
			Collider2D()->Deactivate();
	}
}
