#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ScriptUI
	: public UI
{
private:
	CGameObject*   m_pTargetObject;
	CScript*       m_pTargetScript;
	COMPONENT_TYPE m_eComType;

	bool          m_bActive;
	std::wstring* m_pItemName;
private:
	void ItemSelected(void* pItemName) const;
public:
	void         SetTargetObject(CGameObject* _pTarget) { m_pTargetObject = _pTarget; }
	CGameObject* GetTargetObject() const { return m_pTargetObject; }

	void     SetTargetScript(CScript* _pScript) { m_pTargetScript = _pScript; }
	CScript* GetTargetScript() const { return m_pTargetScript; }

protected:
	bool IsComponentActive() const { return m_bActive; }

public:
	void render_update() override;

public:
	ScriptUI();
	virtual ~ScriptUI();
};
