#pragma once
#include <Engine/CScript.h>


class CCameraScript;

class CMouseScript final
	: public CScript
{
public:
	CMouseScript();
	virtual ~CMouseScript();
	CLONE(CMouseScript);

public:
	void start() override;
	void update() override;

public:
	bool IsMouseIn(CGameObject* pTarget);
	bool IsClicked(CGameObject* pTarget, KEY key);
	bool IsRelease(CGameObject* pTarget, KEY key);
	bool IsLeft() const { return m_isLeft; }
private:
	void UpdatePosition();

private:
	bool           m_isLeft;
	CCameraScript* m_pCameraScript;
	bool m_mouseHovered;
};
