#pragma once
#include "UI.h"

enum class TILE_MAP_TYPE
{
	NONE = -1,
	TILE,
	ONE_WAY_TILE,
	WALL,
	PROP,
	MAX
};

struct DunGreedTileInfo
{
	std::wstring name;
	Vec2         position;
	Vec2         scale;
	Vec2         offset;
	int          tileID;
	bool         oneWay;
	std::wstring tileType;
};


class TileMapToolUI final
	: public UI
{
public:
	TileMapToolUI();
	virtual ~TileMapToolUI();

public:
	void render_update() override;

	void SetTarget(CGameObject* pTarget);

private:
	void OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath);
	void LoadTexture();

	void DrawMenu();
	void DrawTile();
	void DrawTileInspector();

private:
	void DrawCanvasAndGrid();
	void DrawPallet();
	void DrawImage();
private:
	void RoomMetaFromXML();
	void LoadTileMetaInfo();
	//void LoadMap();
private:
	void New();
	void Reset();
	void Save();
	void LoadTileFile();

	void Test();
	void Apply();
	void ApplyTile(const std::wstring& key);
	void ApplyWall(const std::wstring& key);

	void ApplyToTarget();
private:
	CGameObject* m_pTarget;

	std::wstring m_filePath;

	int                                 m_tileCountX;
	int                                 m_tileCountY;
	int                                 m_rowCount;
	int                                 m_colCount;
	ImVec2                              m_vSlicePixel;
	ImVec2                              m_vSliceUV;

	std::vector<std::vector<tTileData>> m_vecTileData;
	std::vector<std::vector<tTileData>> m_vecWallData;

	Ptr<CTexture>                       m_pTexture2D;

	float m_zoomScale;
	bool  m_openTilePallet;
	int   m_prevCountY;
	int   m_selectedTileIndex;

	bool m_openLeft;
	bool m_openRight;
	bool m_openTop;
	bool m_openBottom;

	bool m_isFailed;

	bool m_hasLeft;
	bool m_hasRight;
	bool m_hasTop;
	bool m_hasBottom;

	int m_imageIndex;
	//추출한 타일 데이터 전용
private:
	std::vector<std::vector<int>>                                   m_vecGridCurrentTileID;
	std::vector<std::vector<int>>                                   m_vecCleanTile;

	std::unordered_map<std::wstring, std::vector<std::vector<int>>> m_mapGridTiles;
	std::unordered_map<std::wstring, std::vector<std::vector<int>>> m_mapGridWalls;

	std::unordered_map<int, DunGreedTileInfo>                       m_mapDunGreedTileInfo;

};
