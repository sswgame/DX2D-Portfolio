#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ComponentUI
	: public UI
{
private:
	CGameObject*   m_pTargetObject;
	COMPONENT_TYPE m_eComType;


	bool m_bActive;

public:
	void         SetTargetObject(CGameObject* _pTarget) { m_pTargetObject = _pTarget; }
	CGameObject* GetTargetObject() const { return m_pTargetObject; }

protected:
	bool IsComponentActive() { return m_bActive; }

public:
	void update() override;
	void render_update() override;


public:
	ComponentUI(const string& _strName, COMPONENT_TYPE _eComType);
	virtual ~ComponentUI();
};
