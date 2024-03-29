#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CStructuredBuffer.h"

class CTileMap :
	public CRenderComponent
{
private:
	Ptr<CTexture> m_pAtlasTex;
	Vec2          m_vSlicePixel;
	Vec2          m_vSliceUV;

	UINT m_rowCount;
	UINT m_colCount;

	UINT               m_iTileCountX;
	UINT               m_iTileCountY;
	vector<tTileData>  m_vecTileData;
	CStructuredBuffer* m_pBuffer;

	bool m_bBufferUpdated;


public:
	void SetAtlasTex(Ptr<CTexture> _pAtlasTex)
	{
		m_pAtlasTex = _pAtlasTex;
		m_vSliceUV  = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
	}

	void SetTileSize(Vec2 _vPixelSize)
	{
		m_vSlicePixel = _vPixelSize;
		if (nullptr != m_pAtlasTex)
			m_vSliceUV = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
	}

	void SetTileData(const std::vector<tTileData>& tileData) { m_vecTileData = tileData; }

	void SetTileMapCount(UINT _iCountX, UINT _iCountY);
	void SetTileData(int tileIndex, int imageIndex);
	void ClearTileData();

	Ptr<CTexture>                 GetTexture() const { return m_pAtlasTex; }
	UINT                          GetTileCountX() const { return m_iTileCountX; }
	UINT                          GetTileCountY() const { return m_iTileCountY; }
	Vec2                          GetTileSize() const { return m_vSlicePixel; }
	const std::vector<tTileData>& GetTileDatas() const { return m_vecTileData; }
public:
	void finalupdate() override;
	void render() override;
	void UpdateData() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CTileMap)
public:
	CTileMap();
	CTileMap(const CTileMap& _origin);
	virtual ~CTileMap();
};
