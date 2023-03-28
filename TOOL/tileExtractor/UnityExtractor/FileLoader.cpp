#include "pch.h"
#include "FileLoader.h"
#include "StringHelper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void FileLoader::Run()
{
	std::cout << "PNG �������� �Է��ϼ���(���α׷��� ���� �ٷ� ���� �־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_pngFolderName);

	std::cout << "�ؽ�ó ��Ÿ ������ ��� �������� �Է��ϼ���(���α׷��� ���� �ٷ� ���� �־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_textureMetaFolderName);

	std::cout << "Ÿ�� ��Ÿ �������� �Է��ϼ���(���α׷��� ���� �ٷ� ���� �־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_tileFolderName);

	std::cout << "����� �������� �Է��ϼ���.(������ ����־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_outputFolderName);

	//���� �� ������ �б�
	std::thread readWorker[4]
	{
		std::thread{&FileLoader::ReadPNG, this}
	  , std::thread{&FileLoader::ReadTextureMeta, this}
	  , std::thread{&FileLoader::ReadTileMeta, this}
	  , std::thread{&FileLoader::SetOutput, this}
	};
	for (size_t i = 0; i < std::size(readWorker); ++i)
	{
		readWorker[i].join();
	}

	while (true)
	{
		std::cout << "�м��� �̱� ������/��Ƽ ������ �߿� ������ : (ex: single / multi) " << std::endl;
		std::string input{};
		std::getline(std::cin, input);

		if ("single" == input)
		{
			RunBySingleThread();
			break;
		}
		if ("multi" == input)
		{
			RunByMultiThread();
			break;
		}
		std::cout << "�߸� �Է��ϼ̽��ϴ�. �ٽ� �Է��ϼ���" << std::endl;
		input = {};
	}
}

void FileLoader::ReadPNG()
{
	const std::filesystem::path         filePath = std::filesystem::current_path() / m_pngFolderName;
	std::filesystem::directory_iterator iterator{ filePath };

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;

		m_vecPNGFileName.push_back(entry.path().string());
		++iterator;
	}

	std::ranges::sort(m_vecPNGFileName);
}

void FileLoader::ReadTextureMeta()
{
	const std::filesystem::path         folderPath = std::filesystem::current_path() / m_textureMetaFolderName;
	std::filesystem::directory_iterator iterator{ folderPath };

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;
		m_vecTextureMetaFileName.push_back(entry.path().string());
		++iterator;
	}
	std::ranges::sort(m_vecTextureMetaFileName);
}

void FileLoader::ReadTileMeta()
{
	const std::filesystem::path         filePath = std::filesystem::current_path() / m_tileFolderName;
	std::filesystem::directory_iterator iterator{ filePath };

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;

		m_vecTileFileName.push_back(entry.path().string());
		++iterator;
	}

	std::ranges::sort(m_vecTileFileName);
}

void FileLoader::SetOutput()
{
	const std::filesystem::path folderPath = std::filesystem::current_path() / m_outputFolderName;
	if (false == exists(folderPath))
	{
		const bool result = create_directory(folderPath);
		if (false == result)
		{
			std::cout << "���� �߻�" << "\n";
			std::exit(-1);
		}
	}
	m_outputPath = (folderPath / L"").string();
}

void FileLoader::ExtractSpriteInfo()
{
	if (m_vecPNGFileName.size() != m_vecTextureMetaFileName.size())
	{
		std::cout << "�̹����� ��Ÿ ������ ������ ��ġ���� �ʽ��ϴ�. �ٽ� Ȯ���Ͻʽÿ�" << "\n";
		return;
	}

	for (size_t i = 0; i < m_vecTextureMetaFileName.size(); ++i)
	{
		AtlasInfo  atlasInfo{};
		const auto result = stbi_load(
		                              m_vecPNGFileName[i].c_str()
			, &atlasInfo.width
			, &atlasInfo.height
			, &atlasInfo.channel
			, 0
		);
		assert(result);
		stbi_image_free(result);

		std::stringstream ss{ReadFile(m_vecTextureMetaFileName[i])};
		std::string       line{};

		SpriteInfo spriteInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseSpriteInfoByLine(line, &atlasInfo, &spriteInfo))
			{
				//���� ��������Ʈ�� �̷���� �ؽ�ó�� �ǹ�
				spriteInfo.name       = std::filesystem::path(m_vecPNGFileName[i]).filename().string();
				spriteInfo.posX       = 0;
				spriteInfo.posY       = 0;
				spriteInfo.width      = static_cast<float>(atlasInfo.width);
				spriteInfo.height     = static_cast<float>(atlasInfo.height);
				spriteInfo.pivotX     = 0.5f;
				spriteInfo.pivotY     = 0.5f;
				spriteInfo.internalID = 0;
				break;
			}
			if (m_finishedReadingSprite)
			{
				atlasInfo.mapSpriteInfo.insert({ spriteInfo.internalID, spriteInfo });
				spriteInfo = {};
				m_finishedReadingSprite = false;
			}
		}
		atlasInfo.PNGFileName = std::filesystem::path(m_vecPNGFileName[i]).filename().string();
		m_mapAtlasInfo.insert({ atlasInfo.guid, atlasInfo });
	}
}

void FileLoader::ExtractTileInfo()
{
	for (auto& tileFile : m_vecTileFileName)
	{
		std::stringstream ss{ ReadFile(tileFile) };
		std::string       line{};

		TileInfo tileInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseTileInfoByLine(line, &tileInfo))
			{
				break;
			}
			if (m_finishedReadingFrameInfo)
			{
				m_finishedReadingFrameInfo = false;
				break;
			}
		}
		std::string tileFileName = std::filesystem::path{ tileFile }.filename().string();
		m_mapTileInfo.insert({ tileFileName, tileInfo });
	}
}

void FileLoader::ExtractToFile()
{
	for (auto& [tileFileName, tileInfo] : m_mapTileInfo)
	{
		//1. ���� Ÿ�� ������ �����ϴ� reference guid�� sprite map���� ã�´�.
		auto iter = m_mapAtlasInfo.find(tileInfo.referenceGUID);
		if (iter != m_mapAtlasInfo.end())
		{
			//2. reference id�� �̿��Ͽ� �ش� ��������Ʈ�� ã�´�.
			const auto& spriteInfo = iter->second.mapSpriteInfo[tileInfo.referenceInternalId];

			//3. �ش� ��������Ʈ�� Ÿ�� �� �����͸� �߰��Ͽ�, ���Ϸ� ���
			//4. �� ��, �����ϰ� ��Ʋ�� ���� �̸����� ������ �����, append �������� ������ ���Ͽ� �߰��Ǵ� �������� �۾��Ѵ�.
			//(���� IO ����� �� ũ������, ���Ŀ� ��������)
			const std::string     fileName = iter->second.PNGFileName + ".txt";
			std::filesystem::path filePath = m_outputPath + fileName;
			std::ofstream         outputFile{filePath.string(), std::ios_base::app};
			if (false == outputFile.is_open())
			{
				std::cout << "������ ���µ� �����Ͽ����ϴ�" << std::endl;
				exit(-1);
			}

			WriteToFile(outputFile, tileInfo, spriteInfo, iter->second);
		}
	}
}

void FileLoader::WriteToFile(std::ofstream&    outputFile
                           , const TileInfo&   tileInfo
                           , const SpriteInfo& spriteInfo
                           , const AtlasInfo&  atlas)
{
	//��������Ʈ ���� ���
	//����Ƽ ��ǥ��� �ٸ��Ƿ� �� ���� �����Ͽ� ��� (DX11�� �»���� 0,0 : ����Ƽ�� ���ϴ��� 0,0)
	//name
	outputFile << spriteInfo.name << std::endl;
	//posX
	outputFile << spriteInfo.posX << std::endl;
	//posy
	outputFile << static_cast<float>(atlas.height) - spriteInfo.posY - spriteInfo.height << std::endl;
	//width
	outputFile << spriteInfo.width << std::endl;
	//height
	outputFile << spriteInfo.height << std::endl;
	//
	outputFile << spriteInfo.pivotX << std::endl;
	//pivotY
	outputFile << 1.0f - spriteInfo.pivotY << std::endl;
	//tileID
	outputFile << tileInfo.id << std::endl;
	//oneway
	outputFile << tileInfo.oneWay << std::endl;
	//enumValue
	switch (tileInfo.enumValue)
	{
	case 0:
		outputFile << "BOX" << std::endl;
		break;
	case 1:
		outputFile << "STAIR_TO_RIGHT" << std::endl;
		break;
	case 2:
		outputFile << "STAIR_TO_LEFT" << std::endl;
		break;
	case 3:
		outputFile << "CEILING_TO_LEFT" << std::endl;
		break;
	case 4:
		outputFile << "CEILING_TO_RIGHT" << std::endl;
		break;
	}
}

void FileLoader::RunByMultiThread()
{
	//1.��������Ʈ ���� ����
	m_hardwareCPUCount = std::thread::hardware_concurrency();

	size_t workSize = m_vecTextureMetaFileName.size() / m_hardwareCPUCount;
	size_t remain   = m_vecTextureMetaFileName.size() % m_hardwareCPUCount;

	std::vector<std::thread> vecExtractWorker{};
	vecExtractWorker.reserve(m_hardwareCPUCount);

	for (int i = 0; i < m_hardwareCPUCount; ++i)
	{
		size_t start = i * workSize;
		size_t end = (i != m_hardwareCPUCount - 1) ? (start + workSize) : (start + workSize + remain);
		if (start == end) continue;

		vecExtractWorker.push_back(std::thread{ &FileLoader::ExtractSpriteInfoMulti, this, start, end });
	}

	for (size_t i = 0; i < vecExtractWorker.size(); ++i)
	{
		vecExtractWorker[i].join();
	}

	//2. Ÿ�� ���� ����
	vecExtractWorker.clear();
	workSize = m_vecTileFileName.size() / m_hardwareCPUCount;
	remain   = m_vecTileFileName.size() % m_hardwareCPUCount;

	for (int i = 0; i < m_hardwareCPUCount; ++i)
	{
		size_t start = i * workSize;
		size_t end = (i != m_hardwareCPUCount - 1) ? (start + workSize) : (start + workSize + remain);
		if (start == end) continue;

		vecExtractWorker.push_back(std::thread{ &FileLoader::ExtractTileInfoMulti, this, start, end });
	}

	for (size_t i = 0; i < vecExtractWorker.size(); ++i)
	{
		vecExtractWorker[i].join();
	}
	ExtractToFile();
}

void FileLoader::RunBySingleThread()
{
	ExtractSpriteInfo();
	ExtractTileInfo();
	ExtractToFile();
}

void FileLoader::ExtractSpriteInfoMulti(size_t start, size_t end)
{
	if (m_vecPNGFileName.size() != m_vecTextureMetaFileName.size())
	{
		std::cout << "�̹����� ��Ÿ ������ ������ ��ġ���� �ʽ��ϴ�. �ٽ� Ȯ���Ͻʽÿ�" << "\n";
		return;
	}

	for (size_t i = start; i < end; ++i)
	{
		AtlasInfo  atlasInfo{};
		const auto result = stbi_load(
		                              m_vecPNGFileName[i].c_str()
		                            , &atlasInfo.width
		                            , &atlasInfo.height
		                            , &atlasInfo.channel
		                            , 0
		);

		assert(result);
		stbi_image_free(result);

		std::stringstream ss{ReadFile(m_vecTextureMetaFileName[i])};
		std::string       line{};

		SpriteInfo spriteInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseSpriteInfoByLine(line, &atlasInfo, &spriteInfo))
			{
				//���� ��������Ʈ�� �̷���� �ؽ�ó�� �ǹ�
				spriteInfo.name       = std::filesystem::path(m_vecPNGFileName[i]).filename().string();
				spriteInfo.posX       = 0;
				spriteInfo.posY       = 0;
				spriteInfo.width      = static_cast<float>(atlasInfo.width);
				spriteInfo.height     = static_cast<float>(atlasInfo.height);
				spriteInfo.pivotX     = 0.5f;
				spriteInfo.pivotY     = 0.5f;
				spriteInfo.internalID = 0;
				break;
			}
			if (m_finishedReadingSprite)
			{
				atlasInfo.mapSpriteInfo.insert({ spriteInfo.internalID, spriteInfo });
				spriteInfo = {};
				m_finishedReadingSprite = false;
			}
		}

		atlasInfo.PNGFileName = std::filesystem::path(m_vecPNGFileName[i]).filename().string();

		m_insertMapMutex.lock();
		m_mapAtlasInfo.insert({ atlasInfo.guid, atlasInfo });
		m_insertMapMutex.unlock();
	}
}

void FileLoader::ExtractTileInfoMulti(size_t start, size_t end)
{
	for (size_t i = start; i < end; ++i)
	{
		std::stringstream ss{ReadFile(m_vecTileFileName[i])};
		std::string       line{};

		TileInfo tileInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseTileInfoByLine(line, &tileInfo))
			{
				assert(nullptr);
			}
			if (m_finishedReadingFrameInfo)
			{
				m_finishedReadingFrameInfo = false;
				break;
			}
		}
		m_insertMapMutex.lock();
		std::string tileFileName = std::filesystem::path(m_vecTileFileName[i]).filename().string();
		m_mapTileInfo.insert({ tileFileName, tileInfo });
		m_insertMapMutex.unlock();
	}
}

std::string FileLoader::ReadFile(std::string_view filePath)
{
	std::lock_guard<std::mutex> lock{ m_fileReadMutex };

	std::ifstream inputFile{ filePath.data() };
	std::string   textInFile{};
	if (inputFile.is_open())
	{
		inputFile.seekg(0, std::ios::end);
		const size_t size = inputFile.tellg();
		textInFile.resize(size);
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(textInFile.data(), size);
	}

	return textInFile;
}

bool FileLoader::ParseSpriteInfoByLine(std::string_view input, AtlasInfo* pAtlasInfo, SpriteInfo* pSpriteInfo)
{
	bool              result = false;
	const std::string line   = StringHelper::Trim(input);

	//Parse GUID
	{
		thread_local std::regex  re{ R"(^guid: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pAtlasInfo->guid = StringHelper::Trim(match[1].str());
		}
	}
	//Parse Name
	{
		thread_local std::regex  re{ R"(^name: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			std::string name = StringHelper::Trim(match[1].str());
			if (name.empty())
			{
				return false;
			}
			pSpriteInfo->name = name;
		}
	}
	//Parse Position X
	{
		thread_local std::regex  re{ R"(^x: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->posX = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Position Y
	{
		thread_local std::regex  re{ R"(^y: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->posY = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Width
	{
		thread_local std::regex  re{ R"(^width: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->width = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse height
	{
		thread_local std::regex  re{ R"(^height: (.+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->height = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Pivot
	{
		thread_local std::regex  re{ R"(^pivot\: \{x\: (\d+\.\d+), y\: (\d+\.\d+)\}$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->pivotX = std::stof(StringHelper::Trim(match[1].str()));
			pSpriteInfo->pivotY = std::stof(StringHelper::Trim(match[2].str()));
		}
	}
	//Parse InternalID
	{
		thread_local std::regex  re{ R"(^internalID: (\d+)$)" };
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->internalID = std::stoi(StringHelper::Trim((match[1].str())));
			m_finishedReadingSprite = true;
		}
	}

	return true;
}

bool FileLoader::ParseTileInfoByLine(std::string_view input, TileInfo* pTileInfo)
{
	const std::string line = StringHelper::Trim(input);

	//Parse id
	{
		thread_local std::regex  re{R"(id: (\d+))"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->id = std::stoi(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse width
	{
		thread_local std::regex  re{R"(width: (\d+))"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->width = std::stoi(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse height
	{
		thread_local std::regex  re{R"(height: (\d+))"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->height = std::stoi(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse sprite reference internalID and guid
	{
		thread_local std::regex  re{R"(- \{fileID: (\d+), guid: (.+), .*)"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->referenceGUID       = StringHelper::Trim(match[2].str());
			pTileInfo->referenceInternalId = std::stoi(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse one way
	{
		thread_local std::regex  re{R"(oneWay: (\d?))"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->oneWay = std::stoi(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse shapeType
	{
		thread_local std::regex  re{R"(shapeType: (\d?))"};
		thread_local std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pTileInfo->enumValue = std::stoi(StringHelper::Trim(match[1].str()));
			m_finishedReadingSprite = true;
		}
	}
	return true;
}
