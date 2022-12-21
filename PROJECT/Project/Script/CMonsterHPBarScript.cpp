#include "pch.h"
#include "CMonsterHPBarScript.h"

#include <Engine/CCollider2D.h>

#include "NO_SCRIPT/Define.h"

CMonsterHPBarScript::CMonsterHPBarScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::MONSTERHPBARSCRIPT), DEFINE_NAME(CMonsterHPBarScript)}
  , m_hpRatio{1.f}
  , m_isLeft{false} {}

CMonsterHPBarScript::~CMonsterHPBarScript() = default;

void CMonsterHPBarScript::update()
{
	UpdateData();
}

void CMonsterHPBarScript::UpdateData()
{
	MeshRender()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_hpRatio);

	int isFacingLeft = m_isLeft;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::INT_1, &isFacingLeft);
}

void CMonsterHPBarScript::SetHPRatio(float total, float currentHP)
{
	m_hpRatio = currentHP / total;

	if (m_hpRatio > 1.f)
	{
		m_hpRatio = 1.f;
	}
}
