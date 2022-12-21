#include "pch.h"
#include "CDamageUIScript.h"

#include <Engine/CFontManager.h>

#include "CCameraScript.h"
#include "CObjectManagerScript.h"
#include "CTextScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr float SHOW_DELAY = 1.f;
}

CDamageUIScript::CDamageUIScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::DAMAGEUISCRIPT), DEFINE_NAME(CDamageUIScript)}
  , m_accTime{0}
  , m_pText{nullptr}
  , m_pTextScript{nullptr}
{
	m_keyPrefabText = L"prefab\\TEXT.pref";
}

CDamageUIScript::~CDamageUIScript() = default;

void CDamageUIScript::update()
{
	m_accTime += DT;
	if (m_accTime > SHOW_DELAY)
	{
		m_pTextScript->RemoveFromScene();
		m_pTextScript->Clear();
		CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(m_pText);
		CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(m_pText);
		m_pText       = nullptr;
		m_pTextScript = nullptr;

		CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
		CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
		m_accTime = 0.f;
	}
	if (nullptr != m_pText)
	{
		m_pText->Transform()->SetRelativePos(m_pText->Transform()->GetRelativePos() + Vec2{20.f * DT, 50.f * DT});
	}
}

void CDamageUIScript::ShowDamage(UINT damage)
{
	if (nullptr == m_pText)
	{
		m_pText = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabText);
		m_pText->Transform()->SetRelativePos(Transform()->GetRelativePos());
		CSceneMgr::GetInst()->SpawnObject(m_pText, DEFAULT_LAYER);

		const static Vec4 colors[] = {Colors::Yellow.v, Colors::White.v};
		m_pTextScript = static_cast<CTextScript*>(m_pText->GetScript(static_cast<UINT>(SCRIPT_TYPE::TEXTSCRIPT)));
		m_pTextScript->SetContent(std::to_wstring(damage));
		m_pTextScript->SetColor(colors[GetRandomInRange<size_t>(0, std::size(colors) - 1)]);
	}
}
