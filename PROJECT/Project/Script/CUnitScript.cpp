#include "pch.h"
#include "CUnitScript.h"

#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CEventMgr.h>

CUnitScript::CUnitScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::UNITSCRIPT), DEFINE_NAME(CUnitScript)}
  , m_direction{UNIT_DIRECTION::RIGHT}
  , m_state{}
  , m_isGround{false}
  , m_pGravityScript{nullptr}
  , m_hp{0}
  , m_maxHP{0}
  , m_repeatAnimation{false}
{
	m_maxHP = m_hp;
}

CUnitScript::~CUnitScript() = default;

void CUnitScript::SaveToScene(FILE* _pFile)
{
	CScript::SaveToScene(_pFile);
}

void CUnitScript::LoadFromScene(FILE* _pFile)
{
	CScript::LoadFromScene(_pFile);
}

void CUnitScript::start()
{
	if (nullptr != GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::GRAVITYSCRIPT)))
	{
		m_pGravityScript
			= static_cast<CGravityScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::GRAVITYSCRIPT)));
	}
}

void CUnitScript::update()
{
	UpdateAnimation();

	// 중력 적용
	if (nullptr == m_pGravityScript) { return; }

	if (true == m_isGround)
	{
		// 프레임이 낮을경우 half block 통과 가능성이 있음
		Vec2 velocity = m_pGravityScript->GetVelocity();
		velocity.y    = 0.f;
		m_pGravityScript->SetVelocity(velocity);
		m_pGravityScript->SetGravity(false);
	}
	else
	{
		m_pGravityScript->SetGravity(true);
	}
}

void CUnitScript::lateupdate()
{
	// 유닛 사망 설정
	if (m_hp <= 0)
	{
		m_state = UNIT_STATE::DEAD;
	}
}

void CUnitScript::UpdateAnimation()
{
	if (nullptr != Animator2D())
	{
		auto iter = m_mapAnimation.find(m_state);
		if (iter != m_mapAnimation.end())
		{
			Animator2D()->Play(iter->second, m_repeatAnimation);
		}
	}
		
}

void CUnitScript::SetAnimation(UNIT_STATE state, const std::wstring& animationName)
{
	auto iter = m_mapAnimation.find(state);
	assert(iter == m_mapAnimation.end());
	m_mapAnimation.insert({state, animationName});
}


void CUnitScript::SetUnitDirection(UNIT_DIRECTION direction)
{
	switch (direction)
	{
	case UNIT_DIRECTION::RIGHT:
		{
			Transform()->SetRelativeRotation(Vec3{});
			m_direction = UNIT_DIRECTION::RIGHT;
			break;
		}
	case UNIT_DIRECTION::LEFT:
		{
			Transform()->SetRelativeRotation(Vec3{0.f, XM_PI, 0.f});
			m_direction = UNIT_DIRECTION::LEFT;
			break;
		}
	default:
		break;
	}
}
