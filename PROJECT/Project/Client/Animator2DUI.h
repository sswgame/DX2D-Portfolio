#pragma once
#include "ComponentUI.h"


class CAnimation2D;

class Animator2DUI
	: public ComponentUI
{
private:
	CAnimator2D* m_pAnimator;
	bool         m_isPlaying;
	std::wstring m_selectedAnimationName;
private:
	void PlayAnimation();
	void StopAnimation();
	void EditAnimation();

public:
	void update() override;
	void render_update() override;
public:
	Animator2DUI();
	virtual ~Animator2DUI();
};
