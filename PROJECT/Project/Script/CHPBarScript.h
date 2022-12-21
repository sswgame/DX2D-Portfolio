#pragma once
#include <Engine/CScript.h>


class CTextScript;
class CUnitScript;

class CHPBarScript final
	: public CScript
{
public:
	CHPBarScript();
	virtual ~CHPBarScript();
	CLONE(CHPBarScript);

public:
	void start() override;
	void update() override;
	void UpdateData() override;;

public:
	void SetRemainHP(float hp) { m_remainHP = hp; }
private:
	float m_remainHP;

	CGameObject* m_pPlayer;
	CUnitScript* m_pUnitScript;

	std::wstring m_keyPrefabText;
	CGameObject* m_pText;
	CTextScript* m_pTextScript;
};
