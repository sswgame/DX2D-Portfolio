#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

class CMonsterScript;
class CUnitScript;


class CGiantBatScript final
	: public CScript
{
public:
	CGiantBatScript();
	virtual ~CGiantBatScript();
	CLONE(CGiantBatScript);

public:
	void start() override;
	void update() override;

private:
	void CreateBullet();
	void MoveBullet();

	void CreateBulletNormal();
	void CreateBulletRed();

private:
	float m_attackCoolTime;
	int   m_maxBulletCount;
	int   m_currentBulletCount;

	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;
	CGameObject*    m_pPlayer;

	std::array<CGameObject*, 20> m_arrBullets;
	float                        m_range;
	float                        m_accTime;
	float                        m_bulletTimer;
	float                        m_bulletCreationTerm;
	std::wstring                 m_keyPrefabBullet;
	Vec3                         m_bulletDirection;

	BAT_TYPE m_type;
};
