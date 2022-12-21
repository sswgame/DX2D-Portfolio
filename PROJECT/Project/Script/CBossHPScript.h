#pragma once
#include <Engine/CScript.h>
class CUnitScript;
class CCameraScript;

class CBossHPScript final
	: public CScript
{
public:
	CBossHPScript();
	virtual ~CBossHPScript();
	CLONE(CBossHPScript);

public:
	void start() override;
	void update() override;
	void UpdateData() override;
public:
	void setRemainHP(float hp) { m_remainHP = hp; }

private:
	void UpdatePosition();
private:
	CCameraScript* m_pCameraScript;
	float          m_remainHP;
};
