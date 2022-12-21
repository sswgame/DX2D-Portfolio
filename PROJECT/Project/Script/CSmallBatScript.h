#pragma once
#include <Engine/CScript.h>

#include "CBatBulletScript.h"
#include "NO_SCRIPT/Define.h"


class CMonsterScript;
class CUnitScript;

class CSmallBatScript final
	: public CScript
{
public:
	CSmallBatScript();
	virtual ~CSmallBatScript();
	CLONE(CSmallBatScript);

public:
	void start() override;
	void update() override;

private:
	void CreateBullet();

private:
	float m_attackCoolTime;
	float m_moveCoolTime;
	float m_speed;

	/*CGameObject*    m_pBullet;*/
	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;
	CGameObject*    m_pPlayer;

	float        m_accTime;
	std::wstring m_keyPrefabBullet;
	Vec3         m_bulletDirection;
	float        m_moveDirectionRandom;

	BAT_TYPE m_type;
};
