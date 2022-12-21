#pragma once

#include <Engine/CMgrScript.h>

enum class UI_TYPE
{
	INVENTORY,
	PLAYER_INFO,
	ABILITY,
	STATUS,
	MAP,
	MINI_MAP,
	DIALOGUE,
	RESTAURANT,
	SHOP,
	COSTUME,

	END
};

class CUIManagerScript final
	: public CMgrScript<CUIManagerScript>
{
public:
	CUIManagerScript();
	virtual ~CUIManagerScript();
	CLONE(CUIManagerScript);

public:
	void start() override;

public:
	CGameObject* GetUI(UI_TYPE type);
private:
	std::unordered_map<std::wstring, CGameObject*> m_mapUI;

	CGameObject* m_pMouse;
	bool         m_mouseHovered;
};
