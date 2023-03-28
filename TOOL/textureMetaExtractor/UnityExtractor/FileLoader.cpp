#include "pch.h"
#include "FileLoader.h"
#include "StringHelper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void FileLoader::Progress()
{
	std::cout << "PNG �������� �Է��ϼ���(���α׷��� ���� �ٷ� ���� �־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_pngFolderName);

	std::cout << "�ؽ�ó ��Ÿ ������ ��� �������� �Է��ϼ���(���α׷��� ���� �ٷ� ���� �־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_meta_folderName);

	std::cout << "����� �������� �Է��ϼ���.(������ ����־�� �մϴ�)" << "\n";
	std::getline(std::cin, m_outputFolderName);

	//���� �� ������ �б�
	std::thread readWorker[3]
	{
		std::thread{&FileLoader::ReadPNG, this}
	  , std::thread{&FileLoader::ReadTextureMeta, this}
	  , std::thread{&FileLoader::SetOutput, this}
	};
	for (size_t i = 0; i < std::size(readWorker); ++i)
	{
		readWorker[i].join();
	}

	RunBySingleThread();
	//RunByMultiThread();
}

void FileLoader::ReadPNG()
{
	const std::filesystem::path         filePath = std::filesystem::current_path() / m_pngFolderName;
	std::filesystem::directory_iterator iterator{filePath};

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;

		m_vecPNGFile.push_back(entry.path().string());
		++iterator;
	}

	std::ranges::sort(m_vecPNGFile);
}

void FileLoader::ReadTextureMeta()
{
	const std::filesystem::path         folderPath = std::filesystem::current_path() / m_meta_folderName;
	std::filesystem::directory_iterator iterator{folderPath};

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;
		m_vecTextureMeta.push_back(entry.path().string());
		++iterator;
	}
	std::ranges::sort(m_vecTextureMeta);
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
	if (m_vecPNGFile.size() != m_vecTextureMeta.size())
	{
		std::cout << "�̹����� ��Ÿ ������ ������ ��ġ���� �ʽ��ϴ�. �ٽ� Ȯ���Ͻʽÿ�" << "\n";
		return;
	}

	for (size_t i = 0; i < m_vecTextureMeta.size(); ++i)
	{
		AtlasInfo  atlasInfo{};
		const auto result = stbi_load(
		                              m_vecPNGFile[i].c_str()
		                            , &atlasInfo.width
		                            , &atlasInfo.height
		                            , &atlasInfo.channel
		                            , 0
		                             );
		assert(result);
		stbi_image_free(result);

		std::stringstream ss{ReadFile(m_vecTextureMeta[i])};
		std::string       line{};

		SpriteInfo spriteInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseSpriteInfoByLine(line, &atlasInfo, &spriteInfo))
			{
				//���� ��������Ʈ�� �̷���� �ؽ�ó�� �ǹ�
				spriteInfo.name       = std::filesystem::path(m_vecPNGFile[i]).filename().string();
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
				atlasInfo.mapSpriteInfo[spriteInfo.internalID].push_back(spriteInfo);
				spriteInfo              = {};
				m_finishedReadingSprite = false;
			}
		}
		const std::string pngFileName = std::filesystem::path(m_vecPNGFile[i]).filename().string();
		m_mapAtlasInfo.insert({pngFileName, atlasInfo});
	}
}

void FileLoader::ExtractToFile()
{
	for (auto& [pngFileName, atlasInfo] : m_mapAtlasInfo)
	{
		const std::string     fileName = pngFileName + ".txt";
		std::filesystem::path filePath = m_outputPath + fileName;
		std::ofstream         outputFile{filePath.string()};
		if (false == outputFile.is_open())
		{
			std::cout << "������ ���µ� �����Ͽ����ϴ�" << std::endl;
			exit(-1);
		}

		//�ش� �ؽ�ó �̸� ���
		outputFile << pngFileName << std::endl;

		for (auto& [internalID, vecSpriteInfo] : atlasInfo.mapSpriteInfo)
		{
			//��������Ʈ �̸�
			outputFile << "SPRITE NAME" << std::endl;
			outputFile << vecSpriteInfo[0].name << std::endl;

			if (vecSpriteInfo[0].name == "")
			{
				int a = 0;
			}
			for (const auto& spriteInfo : vecSpriteInfo)
			{
				//����Ƽ ��ǥ��� �����ϴ��� (0,0)�̹Ƿ� ���� ����� ���ϱ� ���� ������ ���� ���� ���
			//X ��ǥ : �������� �����ϴ� ���� �����ϹǷ� ���� X
				outputFile << "X" << std::endl;
				outputFile << spriteInfo.posX << std::endl;

				//Y ��ǥ : �̹��� ���� - ��ǥ - ����
				outputFile << "Y" << std::endl;
				outputFile << atlasInfo.height - spriteInfo.posY - spriteInfo.height << std::endl;

				outputFile << "WIDTH" << std::endl;
				outputFile << spriteInfo.width << std::endl;

				outputFile << "HEIGHT" << std::endl;
				outputFile << spriteInfo.height << std::endl;

				//PIVOT X ��ǥ : ���� �ϴܺ��� (0,0) �̹Ƿ� ����
				outputFile << "OFFSET X" << std::endl;
				outputFile << spriteInfo.pivotX << std::endl;

				//PIVOT Y ��ǥ : 1 - ��ǥ
				outputFile << "OFFSET Y" << std::endl;
				outputFile << 1.0f - spriteInfo.pivotY << std::endl << std::endl;
			}

		}
	}
}

void FileLoader::RunByMultiThread()
{
	//1.��������Ʈ ���� ����
	m_hardwareCPUCount = std::thread::hardware_concurrency();

	const size_t workSize = m_vecTextureMeta.size() / m_hardwareCPUCount;
	const size_t remain   = m_vecTextureMeta.size() % m_hardwareCPUCount;

	std::vector<std::thread> vecExtractWorker{};
	vecExtractWorker.reserve(m_hardwareCPUCount);

	for (int i = 0; i < m_hardwareCPUCount; ++i)
	{
		size_t start = i * workSize;
		size_t end   = (i != m_hardwareCPUCount - 1) ? (start + workSize) : (start + workSize + remain);
		if (start == end) continue;

		vecExtractWorker.push_back(std::thread{&FileLoader::ExtractSpriteInfoMulti, this, start, end});
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
	ExtractToFile();
}

void FileLoader::ExtractSpriteInfoMulti(size_t start, size_t end)
{
	if (m_vecPNGFile.size() != m_vecTextureMeta.size())
	{
		std::cout << "�̹����� ��Ÿ ������ ������ ��ġ���� �ʽ��ϴ�. �ٽ� Ȯ���Ͻʽÿ�" << "\n";
		return;
	}

	for (size_t i = start; i < end; ++i)
	{
		AtlasInfo  atlasInfo{};
		const auto result = stbi_load(
		                              m_vecPNGFile[i].c_str()
		                            , &atlasInfo.width
		                            , &atlasInfo.height
		                            , &atlasInfo.channel
		                            , 0
		                             );
		assert(result);
		stbi_image_free(result);

		std::stringstream ss{ReadFile(m_vecTextureMeta[i])};
		std::string       line{};

		SpriteInfo spriteInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseSpriteInfoByLine(line, &atlasInfo, &spriteInfo))
			{
				//���� ��������Ʈ�� �̷���� �ؽ�ó�� �ǹ�
				spriteInfo.name       = std::filesystem::path(m_vecPNGFile[i]).filename().string();
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
				m_insertVectorMutex.lock();
				atlasInfo.mapSpriteInfo[spriteInfo.internalID].push_back(spriteInfo);
				spriteInfo              = {};
				m_finishedReadingSprite = false;
				m_insertVectorMutex.unlock();
			}
		}

		const std::string pngFileName = std::filesystem::path(m_vecPNGFile[i]).filename().string();

		m_insertMapMutex.lock();
		m_mapAtlasInfo.insert({pngFileName, atlasInfo});
		m_insertMapMutex.unlock();
	}
}

std::string FileLoader::ReadFile(std::string_view filePath)
{
	std::lock_guard<std::mutex> lock{m_fileReadMutex};

	std::ifstream inputFile{filePath.data()};
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

	//Parse Name
	{
		thread_local std::regex re{R"(^name: (.+)$)"};
		std::smatch             match{};
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
		thread_local std::regex re{R"(^x: (.+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->posX = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Position Y
	{
		thread_local std::regex re{R"(^y: (.+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->posY = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Width
	{
		thread_local std::regex re{R"(^width: (.+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->width = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse height
	{
		thread_local std::regex re{R"(^height: (.+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->height = std::stof(StringHelper::Trim(match[1].str()));
		}
	}
	//Parse Pivot
	{
		thread_local std::regex re{R"(^pivot: \{x: (\d+\.\d+|0), y: (\d+\.\d+|0)\}$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->pivotX = std::stof(StringHelper::Trim(match[1].str()));
			pSpriteInfo->pivotY = std::stof(StringHelper::Trim(match[2].str()));
		}
	}
	//Parse InternalID
	{
		thread_local std::regex re{R"(^internalID: (-?\d+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->internalID = std::stoi(StringHelper::Trim((match[1].str())));
			m_finishedReadingSprite = true;
		}
	}

	return true;
}
