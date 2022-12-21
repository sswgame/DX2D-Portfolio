#pragma once
#include <Engine/CAnimator2D.h>
#include <Engine/Cscript.h>

#include "NO_SCRIPT/Define.h"

class CMonsterSummonScript final
	: public CScript
{
public:
	CMonsterSummonScript();
	CMonsterSummonScript(const CMonsterSummonScript& other);
	virtual ~CMonsterSummonScript();
	CLONE(CMonsterSummonScript);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void start() override;
	void update() override;

	void SetDead();

private:
	std::wstring m_keyPrefab;
	bool         m_isDead;
};
