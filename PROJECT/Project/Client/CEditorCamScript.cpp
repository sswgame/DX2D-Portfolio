#include "pch.h"
#include "CEditorCamScript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>

#include <Engine/CTransform.h>
#include <Engine/CCamera.h>
#include <Script/CSceneSaveLoad.h>

CEditorCamScript::CEditorCamScript()
	:
	CScript(-1, L"CEditorCamScript")
  , m_fSpeed(200.f)
  , m_moveAmount{0} {}

CEditorCamScript::~CEditorCamScript() {}

void CEditorCamScript::update()
{
	Vec3 vPos = Transform()->GetRelativePos();
	vPos.z    = -500.f;
	Transform()->SetRelativePos(vPos);
	float fSpeed = m_fSpeed;
	/*if (KEY_PRESSED(KEY::LSHFT))
		fSpeed *= 3.f;*/

	if (KEY_PRESSED(KEY::W) && KEY_PRESSED(KEY::LCTRL))
	{
		vPos.y += fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::S) && KEY_PRESSED(KEY::LCTRL))
	{
		vPos.y -= fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::A) && KEY_PRESSED(KEY::LCTRL))
	{
		vPos.x -= fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::D) && KEY_PRESSED(KEY::LCTRL))
	{
		vPos.x += fSpeed * DT;
	}
	Transform()->SetRelativePos(vPos);

	if (KEY_PRESSED(KEY::_1) && KEY_PRESSED(KEY::LCTRL))
	{
		Camera()->SetWidth(Camera()->GetWidth() - DT * 100.f);
	}

	if (KEY_PRESSED(KEY::_2) && KEY_PRESSED(KEY::LCTRL))
	{
		Camera()->SetWidth(Camera()->GetWidth() + DT * 100.f);
	}

	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::Z))
	{
		Camera()->SetWidth(1600.f);
		Transform()->SetRelativePos(Vec3{0, 0, vPos.z});
		Transform()->SetRelativeRotation(Vec3{});
	}
	if (KEY_PRESSED(KEY::LCTRL) && KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += DT * vMouseDir.x * XM_PI;
		vRot.x -= DT * vMouseDir.y * XM_PI;

		Transform()->SetRelativeRotation(vRot);
	}
}
