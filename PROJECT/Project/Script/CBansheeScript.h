#pragma once
#include <Engine/CScript.h>

class CMonsterScript;
class CUnitScript;

class CBansheeScript final
	: public CScript
{
public:
	CBansheeScript();
	virtual ~CBansheeScript();
	CLONE(CBansheeScript);

public:
	void start() override;
	void update() override;

private:
	void CreateBullet();
private:
	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;
	float           m_accTime;
	std::wstring    m_keyBulletPrefab;
	float           m_coolTime;
};
