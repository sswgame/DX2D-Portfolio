#pragma once
#include "ComponentUI.h"

class TileMapUI
	: public ComponentUI
{
public:
	TileMapUI();
	virtual ~TileMapUI();

public:
	void update() override;
	void render_update() override;
private:
	CTileMap* m_pTileMapComponent;
};
