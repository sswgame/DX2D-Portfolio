#include "pch.h"
#include "CBossHPScript.h"

#include <Engine/CMgrScript.h>
#include <Engine/CCamera.h>

#include "CCameraScript.h"
#include "CObjectManagerScript.h"
#include "CUnitScript.h"
#include "../Engine/Engine/GameShaderInfo.h"

CBossHPScript::CBossHPScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSHPSCRIPT), DEFINE_NAME(CBossHPScript)}
  , m_pCameraScript{nullptr}
  , m_remainHP{1.f} {}

CBossHPScript::~CBossHPScript() = default;

void CBossHPScript::start()
{
	CGameObject* pCamera = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera();
	m_pCameraScript = static_cast<CCameraScript*>(pCamera->GetScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT)));
	Transform()->SetIgnoreParentScale(true);
	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(PLAYER_HP_BAR_SHADER);
	MeshRender()->GetDynamicMaterial()->SetShader(pShader);
}

void CBossHPScript::update()
{
	UpdatePosition();
	UpdateData();
}

void CBossHPScript::UpdateData()
{
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_remainHP);
	float offsetRatio = 0.16f;
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &offsetRatio);
}

void CBossHPScript::UpdatePosition()
{
	Vec3               cameraPos  = m_pCameraScript->Transform()->GetRelativePos();
	static const float halfHeight = m_pCameraScript->Camera()->GetHeight() * 0.5f;

	cameraPos.y -= halfHeight;
	cameraPos.y += 80.f;
	cameraPos.z = 0.f;

	Transform()->SetRelativePos(cameraPos);
}
