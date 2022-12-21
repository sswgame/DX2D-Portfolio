#pragma once
#include <Engine/CScript.h>

class CUnitScript;

class CSkelBowScript final
	: public CScript
{
public:
	CSkelBowScript();
	virtual ~CSkelBowScript();
	CLONE(CSkelBowScript);

public:
	void start() override;
	void update() override;

private:
	CGameObject* m_pPlayer;
	CUnitScript* m_pSkelUnitScript;
	wstring      m_keyPrefabArrow;
};
