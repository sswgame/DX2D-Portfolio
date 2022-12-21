#include "pch.h"
#include "CSkelKnightAttackScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>
#include "CPlayerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	inline const wchar_t* SKEL_KNIGHT_FIRST_ATTACK  = L"sound\\SKEL_SWORD_ATTACK_2.wav";
	inline const wchar_t* SKEL_KNIGHT_SECOND_ATTACK = L"sound\\SKEL_SWORD_ATTACK_3.wav";
}

CSkelKnightAttackScript::CSkelKnightAttackScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELKNIGHTATTACKSCRIPT), DEFINE_NAME(CSkelKnightAttackScript)} {}

CSkelKnightAttackScript::~CSkelKnightAttackScript() = default;

void CSkelKnightAttackScript::update()
{
	const int animationIndex = GetOwner()->GetParent()->Animator2D()->GetCurrentAnim()->GetCurrentFrameIndex();
	if (2 < animationIndex && animationIndex < 6)
	{
		if (false == GetOwner()->IsActive())
		{
			Collider2D()->Activate();
			CResMgr::GetInst()->FindRes<CSound>(SKEL_KNIGHT_FIRST_ATTACK)->Play(1, 1.f);
		}
		//else if(2< Animat)
	}
	if (6 == animationIndex)
	{
		if (true == Collider2D()->IsActive())
		{
			Collider2D()->Deactivate();
		}
	}
	if (6 < animationIndex)
	{
		if (false == Collider2D()->IsActive())
		{
			CResMgr::GetInst()->FindRes<CSound>(SKEL_KNIGHT_SECOND_ATTACK)->Play(1, 1.f);
			Collider2D()->Activate();
		}
	}
}
