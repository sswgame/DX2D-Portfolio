#pragma once

#include "componentUI.h"

class MeshRenderUI :
	public ComponentUI
{
public:
	void update() override;
	void render_update() override;

public:
	void MeshSelect(void* pMeshName);
	void MaterialSelect(void* pMaterialName);

public:
	MeshRenderUI();
	virtual ~MeshRenderUI();

private:
	void DrawMeshButton();
	void DrawMaterialButton();
	void DrawSTDMaterialSettingInfo();
private:
	void*        m_pDroppedData;
	CMeshRender* m_pMeshRender;

	Ptr<CMesh>  m_pMesh;
	std::string m_meshName;

	Ptr<CMaterial> m_pSelectedMaterial;
	std::string    m_materialName;
};
