#pragma once
#include <Engine/CScript.h>

class CAttackScript final
	: public CScript
{
public:
	CAttackScript();
	virtual ~CAttackScript();
	CLONE(CAttackScript);

public:
	void update() override;
	void SetColliderTime(float time) { m_attackAvailableTime = time; }
	bool IsEnd() const { return m_isEnd; }
private:
	bool  m_isEnd;
	float m_attackAvailableTime;
	float m_accTime;
};
