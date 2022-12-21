#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ComponentUI;
class ScriptUI;
class ResInfoUI;
class CRes;


class InspectorUI :
	public UI
{
private:
	CGameObject* m_pTargetObject;
	CRes*        m_pTargetRes;

	ComponentUI* m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::END)];
	ResInfoUI*   m_arrResUI[static_cast<UINT>(RES_TYPE::END)];

	std::vector<ScriptUI*> m_vecScriptUI;

public:
	void SetTargetObject(CGameObject* _pTarget);
	void SetTargetResource(CRes* _pTargetRes);

private:
	ScriptUI* AddScriptUI();

public:
	void update() override;
	void render_update() override;

public:
	InspectorUI();
	virtual ~InspectorUI();
};
