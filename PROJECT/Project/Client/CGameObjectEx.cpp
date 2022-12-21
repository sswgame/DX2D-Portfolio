#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine/CComponent.h>

CGameObjectEx::CGameObjectEx() {}

CGameObjectEx::~CGameObjectEx() {}

void CGameObjectEx::finalupdate()
{
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		CComponent* pCom = GetComponent(static_cast<COMPONENT_TYPE>(i));
		if (nullptr != pCom)
			pCom->finalupdate();
	}

	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		static_cast<CGameObjectEx*>(vecChild[i])->finalupdate();
	}
}
