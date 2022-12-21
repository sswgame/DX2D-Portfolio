#pragma once
#include <Engine/CScript.h>

class CMinotaursAttackScript final
	: public CScript
{
public:
	CMinotaursAttackScript();
	virtual ~CMinotaursAttackScript();
	CLONE(CMinotaursAttackScript);

public:
	void update() override;
};
