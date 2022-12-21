#pragma once
#include <Engine/CScript.h>

class CWarnEffectScript final
	: public CScript
{
public:
	CWarnEffectScript();
	virtual ~CWarnEffectScript();
	CLONE(CWarnEffectScript);

public:
	void update() override;

private:
	float m_accTime;
};
