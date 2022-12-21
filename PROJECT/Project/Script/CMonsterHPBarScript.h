#pragma once
#include <Engine/CScript.h>

class CMonsterHPBarScript final
	: public CScript
{
public:
	CMonsterHPBarScript();
	virtual ~CMonsterHPBarScript();
	CLONE(CMonsterHPBarScript);

public:
	void update() override;
	void UpdateData() override;

public:
	void SetHPRatio(float total, float currentHP);
	void SetFacingLeft(bool isFacingLeft) { m_isLeft = isFacingLeft; }

private:
	float m_hpRatio;
	bool  m_isLeft;
};
