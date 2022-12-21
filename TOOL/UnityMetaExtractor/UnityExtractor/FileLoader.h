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

struct FrameInfo
{
	float      startTime;
	SpriteInfo spriteInfo;
};

struct AnimInfo
{
	std::string            name;
	float                  totalTime;
	bool                   loop;
	std::set<std::string>  vecPNGFiles;
	std::vector<FrameInfo> vecFrames;
};

class FileLoader
{
public:
	FileLoader()  = default;
	~FileLoader() = default;
public:
	void Progress();

	void ReadPNG();
	void ReadTextureMeta();
	void ReadAnimMeta();
	void SetOutput();

	void ExtractSpriteInfo();
	void ExtractSpriteInfoMulti(size_t start, size_t end);
	void ExtractAnimInfo();
	void ExtractAnimInfoMulti(size_t start, size_t end);
	void ExtractToFile();

	void RunByMultiThread();
	void RunBySingleThread();
	
	std::string ReadFile(std::string_view filePath);
	bool        ParseSpriteInfoByLine(std::string_view input, AtlasInfo* pAtlasInfo, SpriteInfo* pSpriteInfo);
	bool        ParseAnimInfoByLine(std::string_view input, AnimInfo* pAnimInfo, FrameInfo* pFrameInfo);
private:
	std::mutex m_insertMapMutex;
	std::mutex m_testMutex;
	std::mutex m_fileReadMutex;

	int        m_hardwareCPUCount;

	std::string m_pngFolderName;
	std::string m_meta_folderName;
	std::string m_animFolderName;
	std::string m_outputFolderName;

	std::vector<std::string> m_vecPNGFile;
	std::vector<std::string> m_vecTextureMeta;
	std::vector<std::string> m_vecAnimFile;

	std::unordered_map<std::string, AnimInfo> m_mapAnimInfo;
	bool                                      m_finishedReadingFrameInfo;

	std::unordered_map<std::string, AtlasInfo> m_mapAtlasInfo;
	bool                                       m_finishedReadingSprite;

	std::string m_outputPath;
};
