#include "pch.h"
#include "CFadeEffectScript.h"
#include <Engine/CGameObject.h>

#include "CObjectManagerScript.h"

CFadeEffectScript::CFadeEffectScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::FADEEFFECTSCRIPT), DEFINE_NAME(CFadeEffectScript)}
  , m_accTime{0}
  , m_fadeTime{0}
  , m_alpha{0}
  , m_fadeType{FADE_TYPE::FADE_IN}
  , m_isFinished{false} {}

CFadeEffectScript::~CFadeEffectScript() = default;

void CFadeEffectScript::start()
{
	Transform()->SetRelativeScale(Vec3{3000.f, 3000.f, 1.f});
}

void CFadeEffectScript::update()
{
	m_accTime += DT;
	if (m_accTime >= m_fadeTime)
	{
		m_isFinished = true;
		CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
		CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
	}

	if (FADE_TYPE::FADE_OUT == m_fadeType)
	{
		m_alpha = m_accTime / m_fadeTime;
	}
	else
	{
		m_alpha = 1.f - m_accTime / m_fadeTime;
	}
	UpdateData();
}

void CFadeEffectScript::UpdateData()
{
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_alpha);
}

void CFadeEffectScript::Reset()
{
	m_alpha      = 0.f;
	m_accTime    = 0.f;
	m_isFinished = false;
	m_fadeTime   = 0.f;
	m_fadeType   = FADE_TYPE::END;
	GetOwner()->Activate();
}
