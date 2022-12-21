#pragma once
#include "ResInfoUI.h"

class MaterialUI :
	public ResInfoUI
{
private:
	TEX_PARAM m_eSelectedTexParam;

public:
	void update() override;
	void render_update() override;

public:
	void TextureSelected(void* pName);

public:
	MaterialUI();
	virtual ~MaterialUI();
};
