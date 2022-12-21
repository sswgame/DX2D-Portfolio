#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

class CPlayerScript;
class CUnitScript;

class CAfterImageScript final
	: public CScript
{
public:
	CAfterImageScript();
	virtual ~CAfterImageScript();
	CLONE(CAfterImageScript);

public:
	void start() override;
	void update() override;
	void lateupdate() override;
	void UpdateData() override;
public:
	void SetAlpha(float alpha) { m_alpha = alpha; }
	void SetNumber(int number) { m_order = number; }
	void ResetObject();
private:
	void CopyTargetData();

private:
	CUnitScript*   m_pUnitScript;
	float          m_holdingTime;
	float          m_alpha;
	int            m_order;
	bool           m_showImage;
	UNIT_DIRECTION m_direction;

	CPlayerScript* m_pPlayerScript;
	CUnitScript*   m_pTargetUnitScript;
};
