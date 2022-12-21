#pragma once
#include <Engine/CScript.h>

class CTextScript;

class CDamageUIScript final
	: public CScript
{
public:
	CDamageUIScript();
	virtual ~CDamageUIScript();
	CLONE(CDamageUIScript);

public:
	void update() override;

public:
	void ShowDamage(UINT damage);

private:
	float        m_accTime;
	std::wstring m_keyPrefabText;

	CGameObject* m_pText;
	CTextScript* m_pTextScript;
};
