#include "pch.h"
#include "CMinotaursAttackScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>

#include "CPlayerScript.h"
#include "NO_SCRIPT/Define.h"

CMinotaursAttackScript::CMinotaursAttackScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MINOTAURSATTACKSCRIPT), DEFINE_NAME(CMinotaursAttackScript)} {}

CMinotaursAttackScript::~CMinotaursAttackScript() = default;

void CMinotaursAttackScript::update()
{
	const int animationIndex = GetOwner()->GetParent()->Animator2D()->GetCurrentAnim()->GetCurrentFrameIndex();
	if (3 < animationIndex)
	{
		if (false == GetOwner()->IsActive())
		{
			Collider2D()->Activate();
		}
	}
}
