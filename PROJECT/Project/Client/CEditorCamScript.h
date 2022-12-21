#pragma once
#include <Engine/CScript.h>

class CEditorCamScript :
	public CScript
{
private:
	float m_fSpeed;
	float m_moveAmount;
public:
	void update() override;


	CLONE(CEditorCamScript);
public:
	CEditorCamScript();
	virtual ~CEditorCamScript();
};
