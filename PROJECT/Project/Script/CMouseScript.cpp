#include "pch.h"
#include "CMouseScript.h"

#include <Engine/CCamera.h>
#include <Engine/CCollider2D.h>

#include "CCameraScript.h"
#include "NO_SCRIPT/Define.h"

CMouseScript::CMouseScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MOUSESCRIPT), DEFINE_NAME(CMouseScript)}
  , m_isLeft{false}
  , m_pCameraScript{nullptr}
  , m_mouseHovered{false} {}

CMouseScript::~CMouseScript() =default;

void CMouseScript::start()
{
	
	CGameObject* pCamera = CSceneMgr::GetInst()->FindObjectByName(L"Main_Camera");
	m_pCameraScript = static_cast<CCameraScript*>(pCamera->GetScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT)));
}

void CMouseScript::update()
{
	if (KEY_TAP(KEY::_1))
	{
		ShowCursor(SW_HIDE);
	}
	if (KEY_TAP(KEY::_2))
	{
		ShowCursor(SW_SHOW);
	}
	UpdatePosition();
}

void CMouseScript::UpdatePosition()
{
	const Vec3 position = Transform()->GetRelativePos();

	Vec2       mousePos      = CKeyMgr::GetInst()->GetMousePos();
	const Vec2 cameraLeftTop = m_pCameraScript->GetLeftTop();
	mousePos.y *= -1;
	Vec2 newPos = cameraLeftTop + mousePos;

	newPos.x = std::clamp(newPos.x, cameraLeftTop.x, cameraLeftTop.x + m_pCameraScript->Camera()->GetWidth());
	newPos.y = std::clamp(newPos.y, cameraLeftTop.y - m_pCameraScript->Camera()->GetHeight(), cameraLeftTop.y);

	m_isLeft = (newPos.x < m_pCameraScript->Transform()->GetRelativePos().x);

	Transform()->SetRelativePos(Vec3{newPos.x, newPos.y, -400.f});
}

bool CMouseScript::IsMouseIn(CGameObject* pTarget)
{
	m_mouseHovered = false;
	const Vec3 checkPosition = Transform()->GetRelativePos();
	const Vec3 position = pTarget->Collider2D()->GetWorldPos();
	const Vec3 scale = pTarget->Collider2D()->GetWorldScale();

	const Vec2 leftTop = Vec2{ position.x - scale.x * .5f, position.y + scale.y * .5f };
	const Vec2 rightBottom = Vec2{ position.x + scale.x * .5f, position.y - scale.y * .5f };
	if (leftTop.x <= checkPosition.x && checkPosition.x <= rightBottom.x
		&& rightBottom.y <= checkPosition.y && checkPosition.y <= leftTop.y)
	{
		m_mouseHovered = true;
	}
	return m_mouseHovered;
}
bool CMouseScript::IsClicked(CGameObject* pTarget, KEY key)
{
	assert(UI_LAYER == pTarget->GetLayerIndex());

	if (m_mouseHovered && KEY_TAP(key))
	{
		return true;
	}
	return false;
}
bool CMouseScript::IsRelease(CGameObject* pTarget, KEY key)
{
	assert(UI_LAYER == pTarget->GetLayerIndex());
	if (m_mouseHovered && KEY_AWAY(key))
	{
		return true;
	}
	return false;
}