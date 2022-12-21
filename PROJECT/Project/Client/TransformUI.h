#pragma once
#include "ComponentUI.h"


class TransformUI :
	public ComponentUI
{
private:
	Vec3 m_vRelativePos;
	Vec3 m_vRelativeScale;
	Vec3 m_vRelativeRot;

	CTransform* m_pTransform;
	std::string m_layerName;
	int         m_selectedIndex;
	bool        m_ignoreParentScale;
	bool        m_active;
public:
	void update() override;
	void render_update() override;

public:
	TransformUI();
	virtual ~TransformUI();
};
