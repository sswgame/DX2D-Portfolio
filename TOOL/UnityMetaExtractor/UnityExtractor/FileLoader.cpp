#include "pch.h"
#include "FileLoader.h"
#include "StringHelper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void FileLoader::Progress()
{
	std::cout << "PNG 폴더명을 입력하세요(프로그램이 폴더 바로 옆에 있어야 합니다)" << "\n";
	std::getline(std::cin, m_pngFolderName);

	std::cout << "텍스처 메타 정보가 담긴 폴더명을 입력하세요(프로그램이 폴더 바로 옆에 있어야 합니다)" << "\n";
	std::getline(std::cin, m_meta_folderName);

	std::cout << "애니메이션 폴더명을 입력하세요(프로그램이 폴더 바로 옆에 있어야 합니다)" << "\n";
	std::getline(std::cin, m_animFolderName);

	std::cout << "출력할 폴더명을 입력하세요.(폴더가 비어있어야 합니다)" << "\n";
	std::getline(std::cin, m_outputFolderName);

	//폴더 내 데이터 읽기
	std::thread readWorker[4]
	{
		std::thread{&FileLoader::ReadPNG, this}
	  , std::thread{&FileLoader::ReadTextureMeta, this}
	  , std::thread{&FileLoader::ReadAnimMeta, this}
	  , std::thread{&FileLoader::SetOutput, this}
	};
	for (size_t i = 0; i < std::size(readWorker); ++i)
	{
		readWorker[i].join();
	}
	/*auto startTime = std::chrono::high_resolution_clock::now();
	RunByMultiThread();
	std::cout << "총 애니메이션 개수 : " << m_vecAnimFile.size() << std::endl;
	std::cout << "총 생성된 애니메이션 개수 : " << m_mapAnimInfo.size() << std::endl;
	auto endTime = std::chrono::high_resolution_clock::now();
	auto diff    = endTime - startTime;
	std::cout << "총 걸린 시간 : " << std::chrono::duration_cast<std::chrono::seconds>(diff).count() << "초" << std::endl;*/

	auto startTime = std::chrono::high_resolution_clock::now();
	RunBySingleThread();
	std::cout << "총 애니메이션 개수 : " << m_vecAnimFile.size() << std::endl;
	std::cout << "총 생성된 애니메이션 개수 : " << m_mapAnimInfo.size() << std::endl;
	auto endTime = std::chrono::high_resolution_clock::now();
	auto diff    = endTime - startTime;
	std::cout << "총 걸린 시간 : " << std::chrono::duration_cast<std::chrono::seconds>(diff).count() << "초" << std::endl;

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

void FileLoader::ReadAnimMeta()
{
	const std::filesystem::path         filePath = std::filesystem::current_path() / m_animFolderName;
	std::filesystem::directory_iterator iterator{filePath};

	while (iterator != end(iterator))
	{
		const std::filesystem::directory_entry& entry = *iterator;

		m_vecAnimFile.push_back(entry.path().string());
		++iterator;
	}

	std::ranges::sort(m_vecAnimFile);
}

void FileLoader::SetOutput()
{
	const std::filesystem::path folderPath = std::filesystem::current_path() / m_outputFolderName;
	if (false == exists(folderPath))
	{
		const bool result = create_directory(folderPath);
		if (false == result)
		{
			std::cout << "오류 발생" << "\n";
			std::exit(-1);
		}
	}
	m_outputPath = (folderPath / L"").string();
}

void FileLoader::ExtractSpriteInfo()
{
	if (m_vecPNGFile.size() != m_vecTextureMeta.size())
	{
		std::cout << "이미지와 메타 파일의 개수가 일치하지 않습니다. 다시 확인하십시오" << "\n";
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
				//단일 스프라이트로 이루어진 텍스처를 의미
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
				atlasInfo.mapSpriteInfo.insert({spriteInfo.internalID, spriteInfo});
				spriteInfo              = {};
				m_finishedReadingSprite = false;
			}
		}

		atlasInfo.PNGFileName = std::filesystem::path(m_vecPNGFile[i]).filename().string();
		m_mapAtlasInfo.insert({atlasInfo.guid, atlasInfo});
	}
}

void FileLoader::ExtractAnimInfo()
{
	for (auto& animFile : m_vecAnimFile)
	{
		std::stringstream ss{ReadFile(animFile)};
		std::string       line{};

		AnimInfo  animInfo{};
		FrameInfo frameInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseAnimInfoByLine(line, &animInfo, &frameInfo))
			{
				break;
			}
			if (m_finishedReadingFrameInfo)
			{
				m_finishedReadingFrameInfo = false;
				break;
			}
		}
		m_insertMapMutex.lock();
		std::string animFileName = std::filesystem::path(animFile).filename().string();
		m_mapAnimInfo.insert({animFileName, animInfo});
		m_insertMapMutex.unlock();
	}
}

void FileLoader::ExtractToFile()
{
	for (auto& [animName, animInfo] : m_mapAnimInfo)
	{
		const std::string     fileName = animName + ".txt";
		std::filesystem::path filePath = m_outputPath + fileName;
		std::ofstream         outputFile{filePath.string()};
		if (false == outputFile.is_open())
		{
			std::cout << "파일을 여는데 실패하였습니다" << std::endl;
			exit(-1);
		}

		// 해당 애니메이션이 참조하는 PNG 파일 목록 출력
		outputFile << "PNG COUNT" << std::endl;
		outputFile << animInfo.vecPNGFiles.size() << std::endl;
		for (auto& pngPath : animInfo.vecPNGFiles)
		{
			outputFile << pngPath << std::endl;
		}

		//애니메이션 이름
		outputFile << "ANIM NAME" << std::endl;
		outputFile << animInfo.name << std::endl;

		//애니메이션 총 플레이 시간
		outputFile << "TOTAL TIME" << std::endl;
		outputFile << animInfo.totalTime << std::endl;

		//애니메이션 루프 여부
		outputFile << "LOOP" << std::endl;
		outputFile << animInfo.loop << std::endl;

		//애니메이션 프레임 정보
		outputFile << "FRAMES COUNT" << std::endl;
		outputFile << animInfo.vecFrames.size() << std::endl << std::endl;

		outputFile << "FRAME INFO" << std::endl;
		for (size_t i = 0; i < animInfo.vecFrames.size(); ++i)
		{
			outputFile << "FRAME NAME" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.name << std::endl;

			outputFile << "DURATION" << std::endl;
			if (i != animInfo.vecFrames.size() - 1)
				outputFile << animInfo.vecFrames[i + 1].startTime - animInfo.vecFrames[i].startTime << std::endl;
			else
				outputFile << animInfo.totalTime - animInfo.vecFrames[i].startTime << std::endl;

			outputFile << "X" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.posX << std::endl;

			outputFile << "Y" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.posY << std::endl;

			outputFile << "WIDTH" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.width << std::endl;

			outputFile << "HEIGHT" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.height << std::endl;

			outputFile << "OFFSET X" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.pivotX << std::endl;

			outputFile << "OFFSET Y" << std::endl;
			outputFile << animInfo.vecFrames[i].spriteInfo.pivotY << std::endl << std::endl;
		}
	}
}

void FileLoader::RunByMultiThread()
{
	//1.스프라이트 정보 추출
	m_hardwareCPUCount = std::thread::hardware_concurrency();

	size_t workSize = m_vecTextureMeta.size() / m_hardwareCPUCount;
	size_t remain   = m_vecTextureMeta.size() % m_hardwareCPUCount;

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

	//2. 애니메이션 정보 추출
	vecExtractWorker.clear();
	workSize = m_vecAnimFile.size() / m_hardwareCPUCount;
	remain   = m_vecAnimFile.size() % m_hardwareCPUCount;

	for (int i = 0; i < m_hardwareCPUCount; ++i)
	{
		size_t start = i * workSize;
		size_t end   = (i != m_hardwareCPUCount - 1) ? (start + workSize) : (start + workSize + remain);
		if (start == end) continue;

		vecExtractWorker.push_back(std::thread{&FileLoader::ExtractAnimInfoMulti, this, start, end});
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
	ExtractAnimInfo();
	ExtractToFile();
}

void FileLoader::ExtractSpriteInfoMulti(size_t start, size_t end)
{
	if (m_vecPNGFile.size() != m_vecTextureMeta.size())
	{
		std::cout << "이미지와 메타 파일의 개수가 일치하지 않습니다. 다시 확인하십시오" << "\n";
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
				//단일 스프라이트로 이루어진 텍스처를 의미
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
				atlasInfo.mapSpriteInfo.insert({spriteInfo.internalID, spriteInfo});
				spriteInfo              = {};
				m_finishedReadingSprite = false;
			}
		}

		atlasInfo.PNGFileName = std::filesystem::path(m_vecPNGFile[i]).filename().string();

		m_insertMapMutex.lock();
		m_mapAtlasInfo.insert({atlasInfo.guid, atlasInfo});
		m_insertMapMutex.unlock();
	}
}

void FileLoader::ExtractAnimInfoMulti(size_t start, size_t end)
{
	for (size_t i = start; i < end; ++i)
	{
		std::stringstream ss{ReadFile(m_vecAnimFile[i])};
		std::string       line{};

		AnimInfo  animInfo{};
		FrameInfo frameInfo{};
		while (std::getline(ss, line, '\n'))
		{
			if (false == ParseAnimInfoByLine(line, &animInfo, &frameInfo))
			{
				break;
			}
			if (m_finishedReadingFrameInfo)
			{
				m_finishedReadingFrameInfo = false;
				break;
			}
		}
		m_insertMapMutex.lock();
		std::string animFileName = std::filesystem::path(m_vecAnimFile[i]).filename().string();
		m_mapAnimInfo.insert({animFileName, animInfo});
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

	//Parse GUID
	{
		thread_local std::regex re{R"(^guid: (.+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pAtlasInfo->guid = StringHelper::Trim(match[1].str());
		}
	}
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
		thread_local std::regex re{R"(^pivot\: \{x\: (\d+\.\d+), y\: (\d+\.\d+)\}$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->pivotX = std::stof(StringHelper::Trim(match[1].str()));
			pSpriteInfo->pivotY = std::stof(StringHelper::Trim(match[2].str()));
		}
	}
	//Parse InternalID
	{
		thread_local std::regex re{R"(^internalID: (\d+)$)"};
		std::smatch             match{};
		if (std::regex_match(line, match, re))
		{
			pSpriteInfo->internalID = std::stoi(StringHelper::Trim((match[1].str())));
			m_finishedReadingSprite = true;
		}
	}

	return true;
}

bool FileLoader::ParseAnimInfoByLine(std::string_view input, AnimInfo* pAnimInfo, FrameInfo* pFrameInfo)
{
	const std::string line = StringHelper::Trim(input);
	//Parse Name
	{
		std::regex  re{R"(m_Name: (.+))"};
		std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pAnimInfo->name = StringHelper::Trim(match[1].str());
		}
	}
	//Parse start time
	{
		std::regex  re{R"(- time: (\d+\.?\d+))"};
		std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			std::string result = StringHelper::Trim(match[1].str());
			if (result.empty())
			{
				return false;
			}
			pFrameInfo->startTime = std::stof(result);
		}
	}
	//Parse matching sprite info
	{
		std::regex  re{R"(value: \{fileID: (\d+), guid: (.+), type: (\d+)\})"};
		std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			int         internalID = std::stoi(StringHelper::Trim(match[1].str()));
			std::string guid       = StringHelper::Trim(match[2].str());

			std::lock_guard<std::mutex> lock{m_testMutex};
			auto                        iter = m_mapAtlasInfo.find(guid);
			if (iter != m_mapAtlasInfo.end())
			{
				auto& atlasInfo  = iter->second;
				auto& spriteInfo = atlasInfo.mapSpriteInfo[internalID];

				pFrameInfo->spriteInfo = spriteInfo;
				//유니티 좌표계는 좌측하단이 (0,0)이므로 좌측 상단을 구하기 위해 다음과 같은 식을 사용
				//X 좌표 : 좌측부터 시작하는 것은 동일하므로 변경 X
				//Y 좌표 : 이미지 높이 - 좌표 - 높이
				//PIVOT X 좌표 : 좌측 하단부터 (0,0) 이므로 동일
				//PIVOT Y 좌표 : 1 - 좌표
				pFrameInfo->spriteInfo.posY   = atlasInfo.height - spriteInfo.posY - spriteInfo.height;
				pFrameInfo->spriteInfo.pivotY = 1.0f - spriteInfo.pivotY;
				pAnimInfo->vecFrames.push_back(*pFrameInfo);
				pAnimInfo->vecPNGFiles.insert(atlasInfo.PNGFileName);
				*pFrameInfo = {};
			}
		}
	}
	//Parse totalTime
	{
		std::regex  re{R"(m_StopTime: (.+))"};
		std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pAnimInfo->totalTime = std::stof(StringHelper::Trim(match[1].str()));
		}
	}//Parse loop
	{
		std::regex  re{R"(m_LoopTime: (.+))"};
		std::smatch match{};
		if (std::regex_match(line, match, re))
		{
			pAnimInfo->loop            = std::stoi(StringHelper::Trim(match[1].str()));
			m_finishedReadingFrameInfo = true;
		}
	}

	return true;
}
