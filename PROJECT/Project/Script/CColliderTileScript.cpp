#include "pch.h"
#include "CColliderTileScript.h"

#include <Engine/CCollider2D.h>
#include "CUnitScript.h"

#include "CMonsterScript.h"
#include "CPlayerScript.h"

CColliderTileScript::CColliderTileScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::COLLIDERTILESCRIPT), DEFINE_NAME(CColliderTileScript)}
  , m_tileType{TILE_TYPE::BLOCK}
  , m_collisionSide{COLLISION_SIDE::NONE}
{
	AddScriptParam("TILE_TYPE", SCRIPTPARAM_TYPE::INT, &m_tileType);
}

CColliderTileScript::~CColliderTileScript() = default;

void CColliderTileScript::start()
{
	m_tileType = static_cast<TILE_TYPE>(std::clamp(static_cast<int>(m_tileType), 0, 1));
}

void CColliderTileScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() == MONSTER_LAYER)
	{
		const CMonsterScript* pMonsterScript = static_cast<CMonsterScript*>(_OtherObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
		if (MONSTER_TYPE::GHOST == pMonsterScript->GetMonsterType())
		{
			return;
		}
		if(MONSTER_TYPE::MINOTAURS == pMonsterScript->GetMonsterType()
		   && TILE_TYPE::HALF_BLOCK==m_tileType)
		{
			return;
		}
	}
	SetOtherPosition(_OtherObject);
}

void CColliderTileScript::OnCollision(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() == MONSTER_LAYER)
	{
		const CMonsterScript* pMonsterScript = static_cast<CMonsterScript*>(_OtherObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::MONSTERSCRIPT)));
		if (MONSTER_TYPE::GHOST == pMonsterScript->GetMonsterType())
		{
			return;
		}
		if(MONSTER_TYPE::MINOTAURS == pMonsterScript->GetMonsterType()
		   && TILE_TYPE::HALF_BLOCK==m_tileType)
		{
			return;
		}
	}
	SetOtherPosition(_OtherObject);
}

void CColliderTileScript::OnCollisionExit(CGameObject* _OtherObject)
{
	// 유닛이 아닐경우 충돌처리 하지 않음
	if (nullptr == _OtherObject->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)))
	{
		return;
	}

	m_collisionSide = COLLISION_SIDE::NONE;

	CUnitScript* pUnitScript = static_cast<CUnitScript*>(_OtherObject->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	pUnitScript->SetGround(false);

	if (_OtherObject->GetLayerIndex() == PLAYER_LAYER)
	{
		CPlayerScript* pScript = static_cast<CPlayerScript*>(_OtherObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));
		pScript->SetHalfTile(false);
	}
}

void CColliderTileScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_tileType, sizeof(TILE_TYPE), 1, _pFile);
	fwrite(&m_collisionSide, sizeof(COLLISION_SIDE), 1, _pFile);
}

void CColliderTileScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_tileType, sizeof(TILE_TYPE), 1, _pFile);
	fread(&m_collisionSide, sizeof(COLLISION_SIDE), 1, _pFile);
}

void CColliderTileScript::SetOtherPosition(CGameObject* pGameObject)
{
	// 유닛이 아닐경우 충돌처리 하지 않음
	if (nullptr == pGameObject->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)))
	{
		return;
	}

	const Vec3& tileColliderPos   = Transform()->GetWorldPos();
	const Vec2& tileColliderScale = Collider2D()->GetOffsetScale();

	const Vec3& targetColliderPos   = pGameObject->Transform()->GetWorldPos();
	const Vec2& targetColliderScale = pGameObject->Collider2D()->GetOffsetScale();

	Vec3 newTargetPos = targetColliderPos;

	CUnitScript* pUnitScript = static_cast<CUnitScript*>(pGameObject->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));

	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(pGameObject->GetLayerIndex());
	if (m_tileType == TILE_TYPE::HALF_BLOCK && L"Player" == pLayer->GetName())
	{
		const CPlayerScript* pPlayerScript = static_cast<CPlayerScript*>(pGameObject->
			// 캐릭터가 위로 점프중이지 않고, 커맨드키를 누르지 않을 경우
			GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));

		const CGravityScript* pGravityScript = static_cast<CGravityScript*>(pGameObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::GRAVITYSCRIPT)));

		if (pUnitScript->IsGround() == false
		    && pGravityScript->GetVelocity().y <= 0.f
		    && pPlayerScript->IsHalfTilePass() == false)
		{
			const float tileTop      = tileColliderPos.y + tileColliderScale.y / 2.f;
			const float targetBottom = targetColliderPos.y - targetColliderScale.y / 2.f;

			//안으로 파고드는 것을 방지
			if (tileTop > targetBottom)
			{
				newTargetPos.y = targetColliderPos.y + (tileTop - targetBottom);
				newTargetPos.y -= pGameObject->Collider2D()->GetOffsetScale().y / 2.f;

				pGameObject->Transform()->SetRelativePos(newTargetPos);
				m_collisionSide = COLLISION_SIDE::TOP;
				pUnitScript->SetGround(true);
			}
		}
	}
	else
	{
		const float tileRight  = tileColliderPos.x + tileColliderScale.x / 2.f;
		const float tileLeft   = tileColliderPos.x - tileColliderScale.x / 2.f;
		const float tileTop    = tileColliderPos.y + tileColliderScale.y / 2.f;
		const float tileBottom = tileColliderPos.y - tileColliderScale.y / 2.f;

		const float targetLeft   = targetColliderPos.x - targetColliderScale.x / 2.f;
		const float targetRight  = targetColliderPos.x + targetColliderScale.x / 2.f;
		const float targetBottom = targetColliderPos.y - targetColliderScale.y / 2.f;
		const float targetTop    = targetColliderPos.y + targetColliderScale.y;

		const Vec3 directionVector = pGameObject->Transform()->GetWorldPos() - pGameObject->Transform()->
		                             GetPrevWorldPos();

		// 1.f 이상이라면 y길이가 더 긴 것이므로, 좌/우에 해당
		// 1.f 보다 작다면 y길이가 더 짧은 것이므로, 아래 위에 해당
		const float aspectRatio = tileColliderScale.y / tileColliderScale.x;
		//1.왼쪽이 부딪힌 경우이므로 밀어내야 한다.
		if (directionVector.x < 0.f
		    && tileRight > targetLeft
		    && aspectRatio > 1.f
		    && targetColliderScale.y <= tileColliderScale.y
		    && targetColliderPos.x > tileColliderPos.x)
		{
			newTargetPos.x = targetColliderPos.x + (tileRight - targetLeft);
			pGameObject->Transform()->SetRelativePos(newTargetPos);
		}
		else if (directionVector.x > 0.f
		         && tileLeft < targetRight
		         && aspectRatio > 1.f
		         && targetColliderScale.y <= tileColliderScale.y
		         && targetColliderPos.x < tileColliderPos.x)
		{
			newTargetPos.x = targetColliderPos.x - (targetRight - tileLeft);
			pGameObject->Transform()->SetRelativePos(newTargetPos);
		}
		//아래쪽에 부딪히는 경우
		else if (directionVector.y < 0.f
		         && tileTop > targetBottom
		         && aspectRatio < 1.f
		         && targetColliderScale.x <= tileColliderScale.x
		         && targetColliderPos.y > tileColliderPos.y)
		{
			newTargetPos.y = targetColliderPos.y + (tileTop - targetBottom);

			newTargetPos.y -= pGameObject->Collider2D()->GetOffsetScale().y / 2.f;

			//중심점이 center_low에 있으므로 그만큼 보정해서 빼줘야한다.
			pGameObject->Transform()->SetRelativePos(newTargetPos);
			pUnitScript->SetGround(true);
		}
		else if (directionVector.y > 0.f
		         && tileBottom < targetTop
		         && aspectRatio < 1.f
		         && targetColliderScale.x <= tileColliderScale.x
		         && targetColliderPos.y < tileColliderPos.y)
		{
			newTargetPos.y = targetColliderPos.y - (targetTop - tileBottom);
			pGameObject->Transform()->SetRelativePos(newTargetPos);
		}
	}
}
