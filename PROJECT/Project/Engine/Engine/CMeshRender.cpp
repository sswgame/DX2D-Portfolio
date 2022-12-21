#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CTransform.h"
#include "CAnimator2D.h"
#include "CScript.h"


CMeshRender::CMeshRender()
	:
	CRenderComponent(COMPONENT_TYPE::MESHRENDER)
  , m_hasMetaInfo{false} {}

CMeshRender::~CMeshRender() {}


void CMeshRender::finalupdate() {}

void CMeshRender::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	Transform()->UpdateData();
	if (1 == m_hasMetaInfo)
	{
		Ptr<CMaterial> pMaterial = GetMaterial();
		pMaterial->SetTexParam(TEX_PARAM::TEX_0, m_pTexture);
		pMaterial->SetScalarParam(SCALAR_PARAM::INT_0, &m_hasMetaInfo);
		pMaterial->SetScalarParam(SCALAR_PARAM::VEC2_0, &m_leftTop);
		pMaterial->SetScalarParam(SCALAR_PARAM::VEC2_1, &m_scale);
		pMaterial->SetScalarParam(SCALAR_PARAM::VEC2_2, &m_backgroundSize);
	}

	GetMaterial()->UpdateData();
	GetMesh()->render();


	if (Animator2D())
	{
		CAnimator2D::Clear();
	}
}

void CMeshRender::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);
	SaveResPtr(m_pTexture, _pFile);
	fwrite(&m_leftTop, sizeof(Vec2), 1, _pFile);
	fwrite(&m_scale, sizeof(Vec2), 1, _pFile);
	fwrite(&m_backgroundSize, sizeof(Vec2), 1, _pFile);
	fwrite(&m_hasMetaInfo, sizeof(bool), 1, _pFile);
	SaveWStringToFile(m_spriteName, _pFile);

}

void CMeshRender::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);
	LoadResPtr(m_pTexture, _pFile);
	fread(&m_leftTop, sizeof(Vec2), 1, _pFile);
	fread(&m_scale, sizeof(Vec2), 1, _pFile);
	fread(&m_backgroundSize, sizeof(Vec2), 1, _pFile);
	fread(&m_hasMetaInfo, sizeof(bool), 1, _pFile);
	LoadWStringFromFile(m_spriteName, _pFile);
}
