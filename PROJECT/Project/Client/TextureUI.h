#pragma once
#include "ResInfoUI.h"

class TextureUI :
	public ResInfoUI
{
private:
public:
	void update() override;
	void render_update() override;

public:
	TextureUI();
	virtual ~TextureUI();
};
