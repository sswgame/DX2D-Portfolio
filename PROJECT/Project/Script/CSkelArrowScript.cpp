#include "pch.h"
#include "CSkelArrowScript.h"

#include <Engine/CAnimator2D.h>

#include "CColliderTileScript.h"
#include "CObjectManagerScript.h"

CSkelArrowScript::CSkelArrowScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::SKELARROWSCRIPT), DEFINE_NAME(CSkelArrowScript)}
  , m_isLeft{false}
  , m_rotationZRadian{0}
  , m_speed{800.f} {}

CSkelArrowScript::~CSkelArrowScript() = default;

void CSkelArrowScript::update()
{
	if (L"ARROW_EFFECT" != Animator2D()->GetCurrentAnim()->GetName())
	{
		Vec3 arrowPosition = Transform()->GetRelativePos();

		arrowPosition.x += m_direction.x * m_speed * DT;
		arrowPosition.y += m_direction.y * m_speed * DT;

		Transform()->SetRelativeRotation(Vec3{0.f, 0.f, m_rotationZRadian});
		Transform()->SetRelativePos(arrowPosition);
	}
	else
	{
		if (Animator2D()->GetCurrentAnim()->IsFinish())
		{
			Animator2D()->GetCurrentAnim()->Reset();
			CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
			CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
		}
	}
}

void CSkelArrowScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetScript(static_cast<UINT>(SCRIPT_TYPE::COLLIDERTILESCRIPT)))
	{
		CColliderTileScript* pTile = static_cast<CColliderTileScript*>(
			_OtherObject->GetScript(static_cast<UINT>(SCRIPT_TYPE::COLLIDERTILESCRIPT)));
		if (TILE_TYPE::HALF_BLOCK == pTile->GetTileType())
		{
			return;
		}
	}

	Animator2D()->Play(L"ARROW_EFFECT", false);
}

void CSkelArrowScript::ResetObject()
{
	Animator2D()->Play(L"MOVE", true);
}
