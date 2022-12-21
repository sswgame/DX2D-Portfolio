#pragma once
#include <Engine/CScript.h>

class CSkelKnightAttackScript final
	: public CScript
{
public:
	CSkelKnightAttackScript();
	virtual ~CSkelKnightAttackScript();
	CLONE(CSkelKnightAttackScript);

public:
	void update() override;
private:
};
