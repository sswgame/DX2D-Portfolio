#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
	:
	m_iLayerIdx(INVALID_LAYER_INDEX) {}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecRoot);
}

void CLayer::start()
{
	for (size_t i = 0; i < m_vecRoot.size(); ++i)
	{
		m_vecRoot[i]->start();
	}
}

void CLayer::update()
{
	for (size_t i = 0; i < m_vecRoot.size(); ++i)
	{
		if (m_vecRoot[i]->IsActive())
			m_vecRoot[i]->update();
	}
}

void CLayer::lateupdate()
{
	for (size_t i = 0; i < m_vecRoot.size(); ++i)
	{
		if (m_vecRoot[i]->IsActive())
			m_vecRoot[i]->lateupdate();
	}
}

void CLayer::finalupdate()
{
	auto iter = m_vecRoot.begin();

	while (iter != m_vecRoot.end())
	{
		(*iter)->finalupdate();

		if ((*iter)->IsDead())
		{
			iter = m_vecRoot.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::DeregisterObject(CGameObject* pTarget)
{
	if (pTarget->GetParent()) { return; }

	const auto iter = std::find_if(m_vecRoot.begin()
	                             , m_vecRoot.end()
	                             , [pTarget](const CGameObject* pGameObject)
	                               {
		                               return pGameObject == pTarget;
	                               });

	if (iter != m_vecRoot.end())
	{
		m_vecRoot.erase(iter);
		pTarget->m_layerIndex = INVALID_LAYER_INDEX;
	}
}
