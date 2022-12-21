#include "pch.h"
#include "CWarnEffectScript.h"

#include <Engine/CMgrScript.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

CWarnEffectScript::CWarnEffectScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::WARNEFFECTSCRIPT), DEFINE_NAME(CWarnEffectScript)}
  , m_accTime{0} {}

CWarnEffectScript::~CWarnEffectScript() = default;

void CWarnEffectScript::update()
{
	m_accTime += DT;
	if (m_accTime > 0.1f)
	{
		CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
		CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
		m_accTime = 0.f;
	}
}
