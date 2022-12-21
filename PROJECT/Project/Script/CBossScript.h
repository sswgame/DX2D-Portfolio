#pragma once
#include <Engine/CScript.h>

#include "CBossHPScript.h"
#include "NO_SCRIPT/Define.h"


class CMonsterScript;
class CUnitScript;

enum class BOSS_PATTERN
{
	RANDOM_GENERATION,
	BULLET,
	LASER,
	SWORD,
	END
};

class CBossScript final
	: public CScript
{
public:
	CBossScript();
	virtual ~CBossScript();
	CLONE(CBossScript);

public:
	void start() override;
	void update() override;

public:
	CBossHPScript* GetHPScript() const { return m_pHPBarScript; }
private:
	void CreateBullet(UINT index);
	void CreateSword();

	void FindAllChild();
private:
	BOSS_PATTERN m_pattern;

	std::wstring m_keyPrefabBullet;
	std::wstring m_keyPrefabSword;

	UINT m_bulletCount;
	UINT m_swordCount;

	CGameObject* m_pLeftHand;
	CGameObject* m_pRightHand;
	CGameObject* m_pDeadParticle;
	CGameObject* m_pHPBar;

	std::deque<CGameObject*> m_queueHandPattern;

	float m_accTime;

	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;

	CBossHPScript* m_pHPBarScript;
};
