#pragma once
#include "ComponentUI.h"

class Light2DUI final
	: public ComponentUI
{
public:
	Light2DUI();
	virtual ~Light2DUI();

public:
	void update() override;
	void render_update() override;

private:
	CLight2D*  m_pLight2D;
	tLightInfo m_lightInfo;
};
