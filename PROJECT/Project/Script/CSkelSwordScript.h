#pragma once
#include <Engine/CScript.h>

class CPlayerScript;

class CSkelSwordScript final
	: public CScript
{
public:
	CSkelSwordScript();
	virtual ~CSkelSwordScript();
	CLONE(CSkelSwordScript);

public:
	void start() override;
	void update() override;
};
