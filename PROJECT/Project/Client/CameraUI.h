#pragma once
#include <bitset>

#include "ComponentUI.h"

class CameraUI final
	: public ComponentUI
{
public:
	void update() override;
	void render_update() override;

public:
	CameraUI();
	virtual ~CameraUI();

private:
	std::bitset<MAX_LAYER> m_layerMask;
	float                  m_maxItemWidth;
	CCamera*               m_pCamera;
};
