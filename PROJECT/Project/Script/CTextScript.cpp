#include "pch.h"
#include "CTextScript.h"

#include <Engine/CCamera.h>
#include <Engine/CMgrScript.h>

#include "CCameraScript.h"
#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"


CTextScript::CTextScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::TEXTSCRIPT), DEFINE_NAME(CTextScript)}
  , m_pTextInfo{nullptr}
  , m_pCameraScript{nullptr}
  , m_waitForOneFrame{false}
  , m_isFirstTime{true}
  , m_setOnlyOnce{false}
{
	m_pTextInfo = new TextInfo{};
}


CTextScript::CTextScript(const CTextScript& other)
	:
	CScript{other}
  , m_pTextInfo{nullptr}
  , m_pCameraScript{other.m_pCameraScript}
  , m_waitForOneFrame{false}
  , m_isFirstTime{true}
  , m_setOnlyOnce{other.m_setOnlyOnce}
{
	m_pTextInfo                = new TextInfo{};
	m_pTextInfo->content       = other.m_pTextInfo->content;
	m_pTextInfo->position      = other.m_pTextInfo->position;
	m_pTextInfo->color         = other.m_pTextInfo->color;
	m_pTextInfo->scale         = other.m_pTextInfo->scale;
	m_pTextInfo->rotationAngle = other.m_pTextInfo->rotationAngle;
}

CTextScript::~CTextScript()
{
	RemoveFromScene();
	SAFE_DELETE(m_pTextInfo);
}

void CTextScript::SaveToScene(FILE* _pFile) { }

void CTextScript::LoadFromScene(FILE* _pFile) {}

void CTextScript::start()
{
	CGameObject* pCamera = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera();
	m_pCameraScript = static_cast<CCameraScript*>(pCamera->GetScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT)));
	InsertToScene();
}

void CTextScript::update()
{
	//·ÎÄÃ ÁÂÇ¥°è
	if (true == m_waitForOneFrame)
	{
		const Vec3 worldPos = Transform()->GetWorldPos();
		const Vec2 leftTop  = m_pCameraScript->GetLeftTop();
		const Vec2 diff     = Vec2{worldPos.x, worldPos.y} - leftTop;

		if (true == m_setOnlyOnce)
		{
			if (m_isFirstTime)
			{
				m_pTextInfo->position = Vec3{fabs(diff.x), fabs(diff.y), 0.f};
				m_isFirstTime         = false;
			}
			else
			{
				return;
			}
		}
		m_pTextInfo->position = Vec3{fabs(diff.x), fabs(diff.y), 0.f};
	}
	m_waitForOneFrame = true;
}

void CTextScript::Clear()
{
	m_pTextInfo->position = Vec3{};
	m_pTextInfo->content.clear();
	m_pTextInfo->color         = Colors::Magenta;
	m_pTextInfo->rotationAngle = 0.f;
	m_pTextInfo->scale         = 1.f;
	m_waitForOneFrame          = false;
	m_setOnlyOnce              = false;
}
