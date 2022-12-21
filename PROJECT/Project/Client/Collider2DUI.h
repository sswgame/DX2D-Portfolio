#pragma once
#include <array>
#include <Engine/CCollider2D.h>

#include "ComponentUI.h"

class Collider2DUI final
	: public ComponentUI
{
public:
	Collider2DUI();
	virtual ~Collider2DUI();

public:
	void update() override;
	void render_update() override;
private:
	CCollider2D*    m_pCollider2D;
	Vec2            m_offsetPos;
	Vec2            m_offsetScale;
	COLLIDER2D_TYPE m_colliderType;

	bool m_arrCheckLayer[MAX_LAYER];

	int         m_selectedIndex;
	std::string m_layerName;

	float m_maxItemWidth;
};
