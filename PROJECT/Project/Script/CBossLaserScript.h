#pragma once
#include <Engine/CScript.h>

class CBossLaserScript final
	: public CScript
{
public:
	CBossLaserScript();
	virtual ~CBossLaserScript();
	CLONE(CBossLaserScript);

public:
	void start() override;
	void update() override;
private:
	void FindChild();
private:
	CGameObject* m_pHead;
	CGameObject* m_pBody;
	bool         m_isDead;
};
