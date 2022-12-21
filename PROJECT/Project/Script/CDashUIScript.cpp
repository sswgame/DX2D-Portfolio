#include "pch.h"
#include "CDashUIScript.h"

#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"

namespace
{
	constexpr float DASH_POS_OFFSET = 50.f;
}

CDashUIScript::CDashUIScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::DASHUISCRIPT), DEFINE_NAME(CDashUIScript)}
  , m_pPlayerScript{nullptr}
  , m_prevMaxDashCount{2}
  , m_prevDashCount{2} {}

CDashUIScript::~CDashUIScript() = default;

void CDashUIScript::start()
{
	m_pPlayerScript
		= static_cast<CPlayerScript*>(CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer()->
			GetScript(static_cast<UINT>(
				          SCRIPT_TYPE::PLAYERSCRIPT)));
	SetMaxDashCount(m_pPlayerScript->GetMaxDashCount());
	m_prevMaxDashCount = m_pPlayerScript->GetMaxDashCount();
	m_prevDashCount    = m_prevMaxDashCount;
}


void CDashUIScript::SetDashCount(int count)
{
	const int maxCount = static_cast<int>(m_vecOuterDashCount.size() + m_vecInnerDashCount.size());
	if (count < 0 || count > maxCount) { return; }
	//1.최대 개수가 2개 이하라면,
	if (maxCount == 2)
	{
		//prev 보다 작다는 의미는 사용했다는 의미이다.
		if (m_prevDashCount > count)
		{
			m_vecOuterDashCount[count]->GetChild()[0]->Deactivate();
		}
		else
		{
			if (count - 1 >= 0)
				m_vecOuterDashCount[count - 1]->GetChild()[0]->Activate();
		}
	}
	//2.최대 개수가 3개 이상이라면
	else
	{
		if (m_prevDashCount > count)
		{
			if (count == 0)
			{
				m_vecOuterDashCount[0]->GetChild()[0]->Deactivate();
			}
			else
			{
				if (count > m_vecInnerDashCount.size())
				{
					m_vecOuterDashCount[1]->GetChild()[0]->Deactivate();
				}
				else
				{
					m_vecInnerDashCount[count - 1]->GetChild()[0]->Deactivate();
				}
			}
		}
		else
		{
			if (count == 1)
			{
				m_vecOuterDashCount[0]->GetChild()[0]->Activate();
			}
			else
			{
				if (count - 1 > m_vecInnerDashCount.size())
				{
					m_vecOuterDashCount[1]->GetChild()[0]->Activate();
				}
				else
				{
					m_vecInnerDashCount[count - 2]->GetChild()[0]->Activate();
				}
			}
		}
	}
	m_prevDashCount = count;
}

void CDashUIScript::SetMaxDashCount(int count)
{
	if (m_vecOuterDashCount.size() == 0)
	{
		Ptr<CPrefab> pPrefab      = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\DASH_OUTSIDE.pref");
		const int    UILayerIndex = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"UI")->GetLayerIdx();
		//LEFT
		CGameObject* pDashOutSide = pPrefab->Instantiate();
		pDashOutSide->Transform()->SetIgnoreParentScale(true);
		pDashOutSide->SetName(pDashOutSide->GetName() + L"_" + std::to_wstring(m_vecOuterDashCount.size()));
		CSceneMgr::GetInst()->SpawnObject(pDashOutSide, UILayerIndex);
		CSceneMgr::GetInst()->AddChild(GetOwner(), pDashOutSide);
		m_vecOuterDashCount.push_back(pDashOutSide);
		//RIGHT
		pDashOutSide = pPrefab->Instantiate();
		pDashOutSide->Transform()->SetIgnoreParentScale(true);
		pDashOutSide->SetName(pDashOutSide->GetName() + L"_" + std::to_wstring(m_vecOuterDashCount.size()));
		CSceneMgr::GetInst()->SpawnObject(pDashOutSide, UILayerIndex);
		CSceneMgr::GetInst()->AddChild(GetOwner(), pDashOutSide);
		pDashOutSide->Transform()->SetRelativeRotation(Vec3{0.f, 0.f, XM_PI});
		m_vecOuterDashCount.push_back(pDashOutSide);
	}

	//코스튬을 바꾸게 되어 최대 개수가 주는 경우, 그만큼이 필요 없으므로, 일단 다 날리고 다시 세팅
	if (m_prevMaxDashCount > count)
	{
		for (int i = 0; i < m_vecInnerDashCount.size(); ++i)
		{
			m_vecInnerDashCount[i]->Destroy();
		}
		m_vecInnerDashCount.clear();
	}

	if (2 < count && count != m_prevMaxDashCount)
	{
		for (int i = 0; i < count - m_prevMaxDashCount; ++i)
		{
			Ptr<CPrefab> pPrefab      = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\DASH_INSIDE.pref");
			const int    UILayerIndex = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"UI")->GetLayerIdx();
			CGameObject* pDashInside  = pPrefab->Instantiate();
			pDashInside->SetName(pDashInside->GetName() + L"_" + std::to_wstring(m_vecOuterDashCount.size()));
			pDashInside->Transform()->SetIgnoreParentScale(true);
			CSceneMgr::GetInst()->SpawnObject(pDashInside, UILayerIndex);
			CSceneMgr::GetInst()->AddChild(GetOwner(), pDashInside);
			m_vecInnerDashCount.push_back(pDashInside);
		}
	}
	m_prevMaxDashCount = static_cast<int>(m_vecOuterDashCount.size() + m_vecInnerDashCount.size());
	UpdatePosition();
}

void CDashUIScript::UpdatePosition()
{
	//OUT LEFT는 변할 필요가 없으므로 그대로 놔둔다.
	for (int i = 0; i < m_vecInnerDashCount.size(); ++i)
	{
		Vec3 position = m_vecInnerDashCount[i]->Transform()->GetRelativePos();
		position.x    = (i + 1) * DASH_POS_OFFSET;
		m_vecInnerDashCount[i]->Transform()->SetRelativePos(position);
	}

	//OUT RIGHT는 다 끝나고 마지막에 설치되어야 하므로, Inner의 개수만큼 옆으로 이동해야한다.
	Vec3 position = m_vecOuterDashCount[1]->Transform()->GetRelativePos();
	position.x    = (m_vecInnerDashCount.size() + 1) * DASH_POS_OFFSET;
	m_vecOuterDashCount[1]->Transform()->SetRelativePos(position);
}
