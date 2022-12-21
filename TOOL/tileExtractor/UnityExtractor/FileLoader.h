#pragma once

struct SpriteInfo
{
	int         internalID{};
	std::string name{};
	float       posX{};
	float       posY{};
	float       width{};
	float       height{};
	float       pivotX{};
	float       pivotY{};
};

struct AtlasInfo
{
	std::string                         PNGFileName;
	std::string                         guid;
	int                                 width;
	int                                 height;
	int                                 channel;
	std::unordered_map<int, SpriteInfo> mapSpriteInfo;
};

struct TileInfo
{
	int         id;
	int         width;
	int         height;
	std::string referenceGUID;
	int         referenceInternalId;
	bool        oneWay;
	int         enumValue;
};

class FileLoader
{
public:
	FileLoader()  = default;
	~FileLoader() = default;
public:
	void Run();

	void ReadPNG();
	void ReadTextureMeta();
	void ReadTileMeta();
	void SetOutput();

	void RunBySingleThread();
	void ExtractSpriteInfo();
	void ExtractTileInfo();

	void RunByMultiThread();
	void ExtractSpriteInfoMulti(size_t start, size_t end);
	void ExtractTileInfoMulti(size_t start, size_t end);

	void ExtractToFile();

	std::string ReadFile(std::string_view filePath);
	void        WriteToFile(std::ofstream&    outputFile
	                      , const TileInfo&   tileInfo
	                      , const SpriteInfo& spriteInfo
	                      , const AtlasInfo&  atlas);
	bool        ParseSpriteInfoByLine(std::string_view input, AtlasInfo* pAtlasInfo, SpriteInfo* pSpriteInfo);
	bool        ParseTileInfoByLine(std::string_view input, TileInfo* pTileInfo);
private:
	std::mutex m_insertMapMutex;
	std::mutex m_fileReadMutex;

	int        m_hardwareCPUCount;

	std::string m_pngFolderName;
	std::string m_textureMetaFolderName;
	std::string m_tileFolderName;
	std::string m_outputFolderName;

	std::vector<std::string> m_vecPNGFileName;
	std::vector<std::string> m_vecTextureMetaFileName;
	std::vector<std::string> m_vecTileFileName;

	std::unordered_map<std::string, TileInfo> m_mapTileInfo;
	bool                                      m_finishedReadingFrameInfo;

	std::unordered_map<std::string, AtlasInfo> m_mapAtlasInfo;
	bool                                       m_finishedReadingSprite;

	std::string m_outputPath;
};
