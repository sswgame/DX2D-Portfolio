#include "pch.h"
#include "CHPBarScript.h"

#include <Engine/CMgrScript.h>

#include "CObjectManagerScript.h"
#include "CTextScript.h"
#include "CUnitScript.h"

CHPBarScript::CHPBarScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::HPBARSCRIPT), DEFINE_NAME(CHPBarScript)}
  , m_remainHP{1.f}
  , m_pPlayer{nullptr}
  , m_pUnitScript{nullptr}
  , m_pText{nullptr}
  , m_pTextScript{nullptr}
{
	m_keyPrefabText = L"prefab\\TEXT.pref";
}

CHPBarScript::~CHPBarScript() = default;

void CHPBarScript::start()
{
	m_pPlayer     = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
	m_pUnitScript = static_cast<CUnitScript*>(m_pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));

	m_pText  = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabText);
	Vec3 pos = Transform()->GetRelativePos();
	pos.y += 20.f;
	pos.x -= 20.f;
	pos.z = -5.f;
	m_pText->Transform()->SetRelativePos(pos);

	m_pTextScript = static_cast<CTextScript*>(m_pText->GetScript(static_cast<UINT>(SCRIPT_TYPE::TEXTSCRIPT)));
	m_pTextScript->SetColor(Colors::White.v);
	m_pTextScript->SetScale(2.f);
	m_pTextScript->SetOnlyOnce(true);
	CSceneMgr::GetInst()->SpawnObject(m_pText, DEFAULT_LAYER);
	CSceneMgr::GetInst()->AddChild(GetOwner(), m_pText);
}

void CHPBarScript::update()
{
	m_remainHP = static_cast<float>(m_pUnitScript->GetHP()) / static_cast<float>(m_pUnitScript->GetMaxHP());
	m_pTextScript->SetContent(std::to_wstring(m_pUnitScript->GetHP()) + L"/" +
	                          std::to_wstring(m_pUnitScript->GetMaxHP()));

	UpdateData();
}

void CHPBarScript::UpdateData()
{
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_remainHP);
	float offsetRatio = 0.28f;
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &offsetRatio);
}
