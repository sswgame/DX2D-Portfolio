#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshRender final
	: public CRenderComponent
{
private:
	Vec2          m_leftTop;
	Vec2          m_scale;
	Vec2          m_backgroundSize;
	Ptr<CTexture> m_pTexture;
	int           m_hasMetaInfo;
	std::wstring  m_spriteName;

public:
	void finalupdate() override;
	void render() override;

public:
	void                SetSpriteName(const std::wstring& name) { m_spriteName = name; }
	const std::wstring& GetSpriteName() const { return m_spriteName; }

	void SetTexture(Ptr<CTexture> pTexture) { m_pTexture = pTexture; }
	void SetHasMetaInfo(int hasMetaInfo) { m_hasMetaInfo = hasMetaInfo; };
	void SetLeftTop(Vec2 leftTop) { m_leftTop = leftTop; };
	void SetScale(Vec2 scale) { m_scale = scale; };
	void SetBackgroundSize(Vec2 backgroundSize) { m_backgroundSize = backgroundSize; };

	Vec2 GetLeftTop() const {return m_leftTop;}
	Vec2 GetScale() const {return m_scale;}
	Vec2 GetBackgroundSize() const {return m_backgroundSize;}
	Ptr<CTexture> GetTexture() const {return m_pTexture;}
public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CMeshRender)

public:
	CMeshRender();
	virtual ~CMeshRender();
};
