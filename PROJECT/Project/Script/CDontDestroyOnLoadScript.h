#pragma once
#include <Engine/CScript.h>

class CDontDestroyOnLoadScript final
	: public CScript
{
public:
	CDontDestroyOnLoadScript();
	virtual ~CDontDestroyOnLoadScript();
	CLONE(CDontDestroyOnLoadScript);

public:
	void start() override;
private:
};
