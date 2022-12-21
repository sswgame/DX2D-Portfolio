#include "pch.h"
#include "AnimationToolUI.h"

#include "ImGui/imgui_stdlib.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

#include <Engine/CCore.h>
#include <Engine/CPathMgr.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CMeshRender.h>

#undef max
#undef min

namespace
{
	float zoom_factor = 0.25f;
}

AnimationToolUI::AnimationToolUI()
	:
	UI{"AnimationTool"}
  , m_spriteInfo{}
  , m_leastPadding{0}
  , m_pAnimTree{nullptr}
  , m_pSelectedNode{nullptr}
  , m_isDrawing{false}
  , m_applyReverseOrder{false}
  , m_isPlaying{false}
  , m_fAccTime(0.f)
  , m_bFinish(false)
  , m_iCurFrmIdx(0)
  , m_leastBase{0}
  , m_repeat{false}
  , m_pTarget{nullptr}
{
	m_pAnimTree = new TreeUI{true};
	m_pAnimTree->SetTitle(u8"ANIMATION LIST");
	m_pAnimTree->UseFrame(false);
	m_pAnimTree->ShowDummyRoot(false);
	m_pAnimTree->UseDragDropOuter(true);
	m_pAnimTree->UseDragDropSelf(true);

	// Clicked Delegate 등록
	m_pAnimTree->SetClickedDelegate(BIND_FN(AnimationToolUI::ItemClicked));
	m_pAnimTree->SetDragAndDropDelegate(BIND_FN(AnimationToolUI::DragAndDropDelegate));

	AddChild(m_pAnimTree);
}

AnimationToolUI::~AnimationToolUI() = default;

void AnimationToolUI::SetTarget(CGameObject* pTarget)
{
	m_pTarget = pTarget;

	if (nullptr != m_pTarget)
	{
		const CAnimator2D* pAnimator    = m_pTarget->Animator2D();
		const auto&        mapAnimation = pAnimator->GetAnimationList();
		for (const auto& [animationName, pAnimation] : mapAnimation)
		{
			if (nullptr == m_pTexture2D)
			{
				m_pTexture2D = pAnimation->GetAtlas();
			}
			//offset을 제외하면, 보간된 값을 사용하므로, 표시할 때에는 픽셀 크기로 바꿔줘야 한다.
			std::vector<SpriteInfo> vecSpriteInfo{};
			vecSpriteInfo.reserve(pAnimation->GetMaxFrame());
			for (UINT i = 0; i < pAnimation->GetMaxFrame(); ++i)
			{
				SpriteInfo spriteInfo{};
				spriteInfo.index          = i;
				spriteInfo.name           = ToString(animationName);
				spriteInfo.backGroundSize = pAnimation->GetBackgroundSize() * m_pTexture2D->GetSize();
				spriteInfo.duration       = pAnimation->GetFrame(i).fDuration;
				spriteInfo.offsetX        = pAnimation->GetFrame(i).vOffset.x * m_pTexture2D->Width();
				spriteInfo.offsetY        = -pAnimation->GetFrame(i).vOffset.y * m_pTexture2D->Height();
				spriteInfo.position       = pAnimation->GetFrame(i).vLT * m_pTexture2D->GetSize();
				spriteInfo.scale          = pAnimation->GetFrame(i).vSlice * m_pTexture2D->GetSize();

				vecSpriteInfo.push_back(std::move(spriteInfo));

				float base  = std::max(spriteInfo.backGroundSize.x, spriteInfo.backGroundSize.y);
				m_leastBase = std::max(base, m_leastBase);
			}
			m_mapAnimations.insert({ToString(animationName), vecSpriteInfo});
		}

		TreeRenew();
	}
}

void AnimationToolUI::DragAndDropDelegate(void* pDragData, void* pDropData)
{
	TreeNode* pDrag = m_pAnimTree->GetDragNode();
	m_pSelectedNode = pDrag;

	TreeNode* pDrop = m_pAnimTree->GetDropNode();
	//드래그 도중 취소하는 경우, pDrop 노드는 null이므 리턴 또는 pDrag 와 pDrop이 같은 경우, 동일 노드에서 일어난 일이므로 리턴
	if (nullptr == pDrop || pDrag == pDrop) return;

	const TreeNode* pDummyDrag = pDrag->GetParent();
	const TreeNode* pDummyDrop = pDrop->GetParent();

	//1. 애니메이션 노드 -> 애니메이션 노드
	if (nullptr == pDummyDrag->GetParent() && nullptr == pDummyDrop->GetParent())
	{
		//애니메이션 노드에서 애니메이션 노드로 이동하는 경우, drag의 모든 SpriteInfo의 이름을 drop의 것으로 교체한 후 아래쪽에 더한다.
		auto& vecDragFrames = *static_cast<std::vector<SpriteInfo>*>(pDragData);
		for (auto& spriteInfo : vecDragFrames)
		{
			spriteInfo.name = pDrop->GetName();
		}
		auto& vecDropFrames = *static_cast<std::vector<SpriteInfo>*>(pDropData);

		vecDropFrames.reserve(vecDropFrames.size() + vecDragFrames.size());
		vecDropFrames.insert(vecDropFrames.end(), vecDragFrames.begin(), vecDragFrames.end());

		//인덱스를 다시 0번부터 부여
		std::transform(vecDropFrames.begin()
		             , vecDropFrames.end()
		             , vecDropFrames.begin()
		             , [i = 0](SpriteInfo& info)mutable
		               {
			               info.index = i++;
			               return info;
		               });
		//drag의 애니메이션 삭제
		m_mapAnimations.erase(pDrag->GetName());
	}
	//2. 애니메이션 노드 -> 프레임 노드
	else if (nullptr == pDummyDrag->GetParent() && nullptr != pDummyDrop->GetParent())
	{
		auto&      vecDragFrames = *static_cast<std::vector<SpriteInfo>*>(pDragData);
		const auto dropInfo      = *static_cast<SpriteInfo*>(pDropData);

		//같은 애니메이션인 경우, 아무일도 일어나지 않음(리턴)
		if (vecDragFrames.front().name == dropInfo.name) return;

		//서로 다른 애니메이션인 경우, 해당 프레임 노드 이후로 추가
		for (auto& dragInfo : vecDragFrames)
		{
			dragInfo.name = dropInfo.name;
		}

		auto iter = m_mapAnimations.find(dropInfo.name);
		assert(iter != m_mapAnimations.end());

		auto& vecDropFrames = iter->second;
		vecDropFrames.reserve(vecDropFrames.size() + vecDragFrames.size());

		vecDropFrames.insert(vecDropFrames.begin() + dropInfo.index, vecDragFrames.begin(), vecDragFrames.end());

		//인덱스를 다시 0번부터 부여
		std::transform(vecDropFrames.begin()
		             , vecDropFrames.end()
		             , vecDropFrames.begin()
		             , [i = 0](SpriteInfo& info)mutable
		               {
			               info.index = i++;
			               return info;
		               });
		//drag의 애니메이션 삭제
		m_mapAnimations.erase(pDrag->GetName());
	}
	//3. 프레임 노드-> 프레임 노드
	else if (nullptr != pDummyDrag->GetParent() && nullptr != pDummyDrop->GetParent())
	{
		SpriteInfo& dragInfo = *static_cast<SpriteInfo*>(pDragData);
		SpriteInfo& dropInfo = *static_cast<SpriteInfo*>(pDropData);

		//서로 같은 애니메이션 이름을 가지고 있다면, 드래그하는 프레임을 드롭 위치와 스왑하겠다는 의미
		if (dragInfo.name == dropInfo.name)
		{
			auto iter = m_mapAnimations.find(dragInfo.name);
			assert(iter != m_mapAnimations.end());
			auto& vecFrames = iter->second;

			std::swap(vecFrames[dragInfo.index], vecFrames[dropInfo.index]);
		}
		//서로 다른 애니메이션 이름을 가지고 있다면,  드래그하는 쪽을 드롭하는 쪽의 하위에 넣고, 드래그 위치에서 뺀다는 의미.
		else
		{
			auto dragIter = m_mapAnimations.find(dragInfo.name);
			assert(dragIter != m_mapAnimations.end());
			auto& vecDragFrames = dragIter->second;

			dragInfo.name = dropInfo.name;
			auto dropIter = m_mapAnimations.find(dropInfo.name);
			assert(dropIter != m_mapAnimations.end());
			auto& vecDropFrames = dropIter->second;
			vecDropFrames.insert(vecDropFrames.begin() + dropInfo.index, dragInfo);

			vecDragFrames.erase(vecDragFrames.begin() + dragInfo.index);
		}
	}
	//4. 프레임 노드-> 애니메이션 노드
	else if (nullptr != pDummyDrag->GetParent() && nullptr == pDummyDrop->GetParent())
	{
		//같은 애니메이션인 경우, 아무런 변화가 없어야하므로 리턴
		const SpriteInfo& dragInfo      = *static_cast<SpriteInfo*>(pDragData);
		auto&             vecDropFrames = *static_cast<std::vector<SpriteInfo>*>(pDropData);
		if (dragInfo.name == vecDropFrames.front().name) return;

		//다른 애니메이션인 경우, 드롭 애니메이션의 마지막으로 해당 프레임을 이동
		vecDropFrames.push_back(dragInfo);
		vecDropFrames.back().name = pDrop->GetName();

		auto dragIter = m_mapAnimations.find(dragInfo.name);
		assert(dragIter != m_mapAnimations.end());
		auto& vecDragFrames = dragIter->second;
		vecDragFrames.erase(vecDragFrames.begin() + dragInfo.index);
	}
	TreeRenew();
}

void AnimationToolUI::Reset()
{
	m_pTexture2D            = nullptr;
	m_leastBase             = 0.f;
	m_filePath              = {};
	m_startPos              = {};
	m_lastPos               = {};
	m_leastPadding          = 0;
	m_pSelectedNode         = nullptr;
	m_isDrawing             = false;
	m_prevBackgroundSize    = {};
	m_backgroundLeftTop     = {};
	m_backgroundRightBottom = {};
	m_isPlaying             = false;
	m_fAccTime              = 0.f;
	m_bFinish               = false;
	m_iCurFrmIdx            = 0;
	m_repeat                = false;
	m_mapAnimations.clear();
	TreeRenew();
}

void AnimationToolUI::render_update()
{
	DrawMenu();
	DrawImage();
	ImGui::SameLine();
	DrawHierarchy();
	ImGui::SameLine();
	DrawSpriteInspector();
	DrawFrames();
	ImGui::SameLine();
	DrawModifyScreen();
	ImGui::SameLine();
	DrawPreviewScreen();
}

void AnimationToolUI::OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath)
{
	wchar_t szName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize    = sizeof(OPENFILENAME);
	ofn.hwndOwner      = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile      = szName;
	ofn.nMaxFile       = sizeof(szName);
	ofn.lpstrFilter    = filterFormat;
	ofn.nFilterIndex   = filterIndex;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle  = 0;

	std::wstring filePath = CPathMgr::GetInst()->GetContentPath();
	filePath += relativePath.c_str();
	ofn.lpstrInitialDir = filePath.c_str();
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetSaveFileName(&ofn))
	{
		m_filePath = ofn.lpstrFile;
	}
}

void AnimationToolUI::DrawMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"Menu"))
		{
			if (ImGui::MenuItem(u8"New")) { New(); }
			if (ImGui::MenuItem(u8"Load")) { Load(); }
			if (ImGui::MenuItem(u8"Save")) { Save(); }
			//if (ImGui::MenuItem(u8"Xml Load")) { LoadXml(); }
			//EndMenu
			ImGui::EndMenu();
		}
		//End MenuBar
		ImGui::EndMenuBar();
	}
}

void AnimationToolUI::New()
{
	//Find Image
	OpenFileDialog(TEXTURE_FILTER_FORMAT, 0, L"texture");
	if (m_filePath.empty()) return;

	//1. 해당 파일이 contentPath 내의 texture 폴더 밑에 존재하는지 확인
	const std::wstring fileName     = std::filesystem::path{m_filePath}.filename();
	const std::wstring relativePath = L"texture\\" + fileName;
	m_pTexture2D                    = CResMgr::GetInst()->FindRes<CTexture>(relativePath);
	//만약 존재하지 않는다면, 해당 텍스처를 로딩 후 texture폴더 밑으로 복사
	if (nullptr == m_pTexture2D)
	{
		m_pTexture2D = LoadTexture();
	}
}

Ptr<CTexture> AnimationToolUI::LoadTexture() const
{
	std::wstring extension = std::filesystem::path{m_filePath}.extension();
	std::transform(extension.begin(), extension.end(), extension.begin(), [](wchar_t c) { return towlower(c); });

	HRESULT      hr{};
	ScratchImage image{};

	if (L".dds" == extension)
		hr = LoadFromDDSFile(m_filePath.c_str(), DDS_FLAGS_NONE, nullptr, image);
	else if (L".tga" == extension)
		hr = LoadFromTGAFile(m_filePath.c_str(), nullptr, image);
	else // WIC (.png, jpg, jpeg, bmp...)
		hr = LoadFromWICFile(m_filePath.c_str(), WIC_FLAGS_NONE, nullptr, image);

	assert(S_OK == hr);

	//외부에 있는 텍스처 또는 로딩이 되지 않은 텍스처이므로, content/texture 밑으로 복사

	const std::wstring fileName     = std::filesystem::path{m_filePath}.filename();
	const std::wstring contentPath  = CPathMgr::GetInst()->GetContentPath();
	const std::wstring relativePath = L"texture\\" + fileName;
	hr                              = SaveToWICFile(image.GetImages()
	                                              , image.GetImageCount()
	                                              , WIC_FLAGS_NONE
	                                              , GetWICCodec(WIC_CODEC_PNG)
	                                              , std::wstring{contentPath + relativePath}.c_str());
	assert(S_OK == hr);
	Ptr<CTexture> pTexture = CResMgr::GetInst()->Load<CTexture>(relativePath, relativePath, false);

	return pTexture;
}

void AnimationToolUI::ApplyToTarget()
{
	if (nullptr != m_pTarget && nullptr != m_pSelectedNode)
	{
		CAnimator2D* pAnimator = m_pTarget->Animator2D();
		pAnimator->RemoveAllAnimation();
		//넣어줄 때에는 0~1 사이에서 보간된 자료로 변환하여 넘겨줘야한다.
		for (auto& [animationName, vecSpriteInfo] : m_mapAnimations)
		{
			std::vector<tAnim2DFrame> vecFrames{};
			vecFrames.reserve(vecSpriteInfo.size());

			
			for (const auto& spriteInfo : vecSpriteInfo)
			{
				tAnim2DFrame frame{};
				frame.fDuration = spriteInfo.duration;
				frame.vLT       = Vec2{spriteInfo.position.x, spriteInfo.position.y} / m_pTexture2D->GetSize();
				frame.vOffset   = Vec2{spriteInfo.offsetX, -spriteInfo.offsetY} / m_pTexture2D->GetSize();
				frame.vSlice    = Vec2{spriteInfo.scale.x, spriteInfo.scale.y} / m_pTexture2D->GetSize();

				vecFrames.push_back(std::move(frame));
			}
			
			std::wstring  name       = ToWString(animationName);
			CAnimation2D* pAnimation = pAnimator->FindAnim(name);
			//없는 애니메이션이라면 추가
			if (nullptr == pAnimation)
			{
				pAnimation = new CAnimation2D{};
				pAnimator->SetAnimation(ToWString(animationName), pAnimation);
			}
			//애니메이션 세팅
			pAnimation->SetName(name);
			pAnimation->Create(m_pTexture2D, Vec2{m_leastBase, m_leastBase}, std::move(vecFrames));
		}
		if (nullptr == pAnimator->GetCurrentAnim())
		{
			const std::string playName = m_mapAnimations.begin()->second.front().name;
			pAnimator->Play(ToWString(playName), true);
		}
		/*if (nullptr != m_pTarget->MeshRender())
		{
			m_pTarget->MeshRender()->GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pTexture2D);
		}*/
	}

	Deactivate();
}


void AnimationToolUI::Save()
{
	OpenFileDialog(ANIM_FILTER_FORMAT, 0, L"animation");
	if (m_filePath.empty()) return;

	std::ofstream outputFile{m_filePath, std::ios_base::binary};
	if (outputFile.is_open())
	{
		//애니메이션 사용자(컨트롤러)이름 길이 저장
		const std::wstring fileName       = std::filesystem::path{m_filePath}.filename();
		int                fileNameLength = static_cast<int>(fileName.size());
		outputFile.write((const char*)&fileNameLength, sizeof(int));
		//애니메이션 사용자(컨트롤러)이름 저장
		outputFile.write((const char*)fileName.c_str(), sizeof(wchar_t) * fileNameLength);
		//애니메이션 카운트 저장
		int animationCount = static_cast<int>(m_mapAnimations.size());
		outputFile.write((const char*)&animationCount, sizeof(int));
		//개별 애니메이션 정보 저장
		for (auto& [animationName, vecFrames] : m_mapAnimations)
		{
			//애니메이션 이름 카운트 저장
			int animNameSize = static_cast<int>(animationName.size());
			outputFile.write((const char*)&animNameSize, sizeof(int));
			//애니메이션 이름 저장
			std::wstring wstrAnimName = ToWString(animationName);
			outputFile.write((const char*)wstrAnimName.data(), sizeof(wchar_t) * animNameSize);
			//-프레임 카운트 저장
			int frameCount = static_cast<int>(vecFrames.size());
			outputFile.write((const char*)&frameCount, sizeof(int));
			//-프레임 데이터 저장(leftTop,slice(scale),offset,duration)
			for (const auto& spriteInfo : vecFrames)
			{
				//leftTop
				outputFile.write((const char*)&spriteInfo.position.x, sizeof(float));
				outputFile.write((const char*)&spriteInfo.position.y, sizeof(float));
				//slice
				outputFile.write((const char*)&spriteInfo.scale.x, sizeof(float));
				outputFile.write((const char*)&spriteInfo.scale.y, sizeof(float));
				//offset (엔진에서 float으로 불러오고 저장하므로 float으로 저장)
				float offsetX = spriteInfo.offsetX;
				float offsetY = spriteInfo.offsetY;
				outputFile.write((const char*)&offsetX, sizeof(float));
				outputFile.write((const char*)&offsetY, sizeof(float));
				//duration
				outputFile.write((const char*)&spriteInfo.duration, sizeof(float));
			}
			//-백그라운드 사이즈 저장
			outputFile.write((const char*)&m_leastBase, sizeof(float));
			outputFile.write((const char*)&m_leastBase, sizeof(float));
			//-아틀라스 null 여부 저장
			bool hasAtlas(nullptr == m_pTexture2D);
			outputFile.write((const char*)&hasAtlas, sizeof(bool));
			//-엔진에서 사용될 키와 상대 경로 (모두 상대경로여야함)
			int keyLength = static_cast<int>(m_pTexture2D->GetKey().size());
			outputFile.write((const char*)&keyLength, sizeof(int));
			outputFile.write((const char*)m_pTexture2D->GetKey().data(), sizeof(wchar_t) * keyLength);

			int pathLength = static_cast<int>(m_pTexture2D->GetRelativePath().size());
			outputFile.write((const char*)&pathLength, sizeof(int));
			outputFile.write((const char*)m_pTexture2D->GetRelativePath().data(), sizeof(wchar_t) * pathLength);
		}

		//현재 애니메이션을 저장(툴에서는 디폴트가 없음)
		int animNameSize = 0;
		outputFile.write((const char*)&animNameSize, sizeof(int));
		std::wstring currentAnim = L"";
		outputFile.write((const char*)currentAnim.data(), sizeof(wchar_t) * 1);

		//애니메이터 반복여부 저장
		outputFile.write((const char*)&m_repeat, sizeof(bool));
	}
	else
	{
		assert(nullptr);
	}
}

void AnimationToolUI::Load()
{
	OpenFileDialog(ANIM_META_FILTER_FORMAT, 0, L"meta");
	if (m_filePath.empty()) return;

	StopPreview();
	m_pSelectedNode = nullptr;
	m_spriteInfo    = {};
	m_startPos      = ImVec2{};
	m_lastPos       = ImVec2{};
	m_leastPadding  = 0;
	m_mapAnimations.clear();
	TreeRenew();

	std::wstring extension = std::filesystem::path{m_filePath}.extension();
	std::transform(extension.begin(), extension.end(), extension.begin(), [](wchar_t c) { return towlower(c); });

	if (L".txt" == extension)
	{
		LoadFromMeta();
	}
	else
	{
		LoadFromAnim();
	}
}

void AnimationToolUI::LoadFromMeta()
{
	std::ifstream inputFile{m_filePath};
	if (inputFile.is_open())
	{
		std::string textInFile{};
		inputFile.seekg(0, std::ios::end);
		const size_t size = inputFile.tellg();
		textInFile.resize(size);
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(textInFile.data(), size);

		std::stringstream ss{textInFile};

		std::string line{};

		SpriteInfo info{};

		while (std::getline(ss, line, '\n'))
		{
			if (nullptr == m_pTexture2D && line.find(".png") != line.npos)
			{
				std::wstring filePath = L"texture\\" + ToWString(line);
				m_pTexture2D          = CResMgr::GetInst()->Load<CTexture>(filePath, filePath, false);
				assert(nullptr != m_pTexture2D);
			}
			else if ("SPRITE NAME" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.name = line;
			}
			else if ("X" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.position.x = std::stof(line);
			}
			else if ("Y" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.position.y = std::stof(line);
			}
			else if ("WIDTH" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.scale.x = std::stof(line);
			}
			else if ("HEIGHT" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.scale.y = std::stof(line);
			}
			else if ("OFFSET X" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.offsetX = info.scale.x * (std::stof(line) - 0.5f);
			}
			else if ("OFFSET Y" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.offsetY = info.scale.y * (std::stof(line) - 0.5f);

				//스프라이트 하나의 정보가 가져왔으므로, 이것을 가공하여 트리에 출력
				float spriteBackgroundLength = std::max(info.scale.x, info.scale.y);
				m_leastBase = (spriteBackgroundLength > m_leastBase) ? spriteBackgroundLength : m_leastBase;
				info.backGroundSize = ImVec2{m_leastBase, m_leastBase};
				info.duration = 0.1f;

				m_spriteInfo = info;
				Append();
			}
		}

		ChangeCheckAllBackgroundSize();
	}
	else
	{
		assert(nullptr);
	}
}

void AnimationToolUI::LoadFromAnim()
{
	std::ifstream inputFile{m_filePath, std::ios_base::binary};
	if (inputFile.is_open())
	{
		//애니메이션 사용자(컨트롤러)이름 길이 불러오기
		std::wstring fileName{};
		int          fileNameSize{};
		inputFile.read((char*)&fileNameSize, sizeof(int));
		//애니메이션 사용자(컨트롤러)이름 불러오기
		fileName.resize(fileNameSize);
		inputFile.read((char*)&fileName[0], sizeof(wchar_t) * fileNameSize);
		//애니메이션 카운트 불러오기
		int animCount{};
		inputFile.read((char*)&animCount, sizeof(int));
		//개별 애니메이션 정보 불러오기
		for (int i = 0; i < animCount; ++i)
		{
			//애니메이션 이름 사이즈 불러오기
			int animNameSize{};
			inputFile.read((char*)&animNameSize, sizeof(int));
			//애니메이션 이름 불러오기
			std::wstring wstrAnimName{};
			wstrAnimName.resize(animNameSize);
			inputFile.read((char*)&wstrAnimName[0], sizeof(wchar_t) * animNameSize);
			std::string animName = ToString(wstrAnimName);
			//-프레임 데이터 불러오기(leftTop,slice(scale),offset,duration)
			//-프레임 카운트 불러오기
			int frameCount{};
			inputFile.read((char*)&frameCount, sizeof(int));

			std::vector<SpriteInfo> vecFrames{};
			vecFrames.resize(frameCount);

			int index = 0;
			for (auto& spriteInfo : vecFrames)
			{
				//leftTop
				spriteInfo.name = animName;
				inputFile.read((char*)&spriteInfo.position.x, sizeof(float));
				inputFile.read((char*)&spriteInfo.position.y, sizeof(float));
				//slice
				inputFile.read((char*)&spriteInfo.scale.x, sizeof(float));
				inputFile.read((char*)&spriteInfo.scale.y, sizeof(float));
				//offset
				inputFile.read((char*)&spriteInfo.offsetX, sizeof(float));
				inputFile.read((char*)&spriteInfo.offsetY, sizeof(float));
				//duration
				inputFile.read((char*)&spriteInfo.duration, sizeof(float));
				spriteInfo.index = index++;
			}

			//-백그라운드 사이즈 불러오기
			ImVec2 backgroundSize{};
			inputFile.read((char*)&backgroundSize.x, sizeof(float));
			inputFile.read((char*)&backgroundSize.y, sizeof(float));
			std::transform(vecFrames.begin()
			             , vecFrames.end()
			             , vecFrames.begin()
			             , [backgroundSize](SpriteInfo& info)
			               {
				               info.backGroundSize = backgroundSize;
				               return info;
			               });
			//백그라운드 사이즈가 모두 동일하므로
			m_leastBase = backgroundSize.x;

			//-아틀라스 null 여부 불러오기
			bool isAtlasNull{};
			inputFile.read((char*)&isAtlasNull, sizeof(bool));
			//-엔진에서 사용될 키와 상대 경로 (모두 상대경로여야함)
			if (false == isAtlasNull)
			{
				int keyNameLength{};
				inputFile.read((char*)&keyNameLength, sizeof(int));
				std::wstring key{};
				key.resize(keyNameLength);
				inputFile.read((char*)&key[0], sizeof(wchar_t) * keyNameLength);

				int relativePathNameLength{};
				inputFile.read((char*)&relativePathNameLength, sizeof(int));
				std::wstring relativePath{};
				relativePath.resize(relativePathNameLength);
				inputFile.read((char*)&relativePath[0], sizeof(wchar_t) * relativePathNameLength);

				m_pTexture2D = CResMgr::GetInst()->Load<CTexture>(key, relativePath, false);
			}

			m_mapAnimations.insert({animName, std::move(vecFrames)});
		}

		//현재 애니메이션 정보 불러오기
		int currentAnimNameSize{};
		inputFile.read((char*)&currentAnimNameSize, sizeof(int));

		std::wstring wstrCurrentName{};
		wstrCurrentName.resize(currentAnimNameSize);
		inputFile.read((char*)&wstrCurrentName[0], sizeof(wchar_t) * currentAnimNameSize);
		if (currentAnimNameSize > 0)
		{
			std::string currentAnimationName = ToString(wstrCurrentName);
			auto        iter                 = m_mapAnimations.find(currentAnimationName);
			assert(iter != m_mapAnimations.end());
			m_spriteInfo = iter->second.front();
		}
		//툴에서 작업해서 값이 없을 수도 있으므로, 한칸 전진
		else
		{
			wchar_t dump{};
			inputFile.read((char*)&dump, sizeof(wchar_t) * 1);
		}
		//애니메이터 반복 여부 불러오기
		inputFile.read((char*)&m_repeat, sizeof(bool));
		TreeRenew();
	}
	else
	{
		assert(nullptr);
	}
}

void AnimationToolUI::DrawImage()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 4;
	const float  width      = windowSize.x * 0.5f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (2.f / 3.f);
	ImGui::BeginChild(u8"ANIMATION_IMAGE_SCREEN"
	                , ImVec2{width, height}
	                , true
	                , ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

	if (nullptr != m_pTexture2D)
	{
		ImGui::Image(m_pTexture2D->GetSRV().Get(), m_pTexture2D->GetSize());
		CropImageByDrag();
		ShowBackGroundSize();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}

void AnimationToolUI::CropImageByDrag()
{
	const ImVec2 windowPos = ImGui::GetWindowPos();

	const bool isHovered = ImGui::IsItemHovered();
	//clicked first
	if (true == isHovered && false == m_isDrawing && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_isDrawing = true;
		m_startPos  = ImGui::GetMousePos() - windowPos;
	}

	//still drawing
	if (m_isDrawing)
	{
		const ImVec2 currentPos = ImGui::GetMousePos() - windowPos;

		//mouse up
		if (false == ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_isDrawing = false;
			m_lastPos   = currentPos;
		}
		//draw rect
		ImDrawList* const pDrawList = ImGui::GetWindowDrawList();
		pDrawList->AddRect(m_startPos + windowPos, currentPos + windowPos, IM_COL32(0, 255, 0, 255));
	}
}

void AnimationToolUI::ShowBackGroundSize()
{
	if (true == m_isDrawing) return;

	const ImVec2 cropSize        = m_lastPos - m_startPos;
	const ImVec2 cropImageCenter = m_startPos + cropSize / 2.f;
	float        baseSize        = std::max(cropSize.x, cropSize.y) + m_leastPadding * 2;
	baseSize                     = (m_leastBase >= baseSize) ? m_leastBase : baseSize;
	m_leastBase                  = baseSize;
	m_spriteInfo.backGroundSize  = ImVec2{baseSize, baseSize};
	m_backgroundLeftTop          = cropImageCenter - m_spriteInfo.backGroundSize / 2.f;
	m_backgroundRightBottom      = cropImageCenter + m_spriteInfo.backGroundSize / 2.f;

	const ImVec2 windowPos = ImGui::GetWindowPos();
	ImDrawList*  pDrawList = ImGui::GetWindowDrawList();

	//Draw BackGround
	pDrawList->AddRectFilled(m_backgroundLeftTop + windowPos
	                       , m_backgroundRightBottom + windowPos
	                       , IM_COL32(3, 252, 244, 128)
	                       , 0);

	//Draw Selected Area
	pDrawList->AddRect(m_startPos + windowPos, m_lastPos + windowPos, IM_COL32(245, 96, 66, 255));
}

void AnimationToolUI::DrawHierarchy()
{
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 2;
	const float  width      = windowSize.x * 0.25f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (2.f / 3.f);
	ImGui::BeginChild(u8"ANIMATION_HIERARCHY_SCREEN", ImVec2{width, height}, true);

	m_pAnimTree->render_update();

	ImGui::EndChild();
}

void AnimationToolUI::DrawSpriteInspector()
{
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 2;
	const float  width      = windowSize.x * 0.25f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (2.f / 3.f);
	ImGui::BeginChild(u8"ANIMATION_INSPECTOR_SCREEN", ImVec2{width, height}, true);

	//Draw Sprite Info
	ImGui::BeginGroup();
	{
		ImGui::Text(u8"SPRITE INFO");

		ImGui::Text(u8"X : %.3f", m_spriteInfo.position.x);
		ImGui::SameLine(100);

		ImGui::Text(u8"WIDTH : %.3f", m_spriteInfo.scale.x);

		ImGui::Text(u8"Y : %.3f", m_spriteInfo.position.y);
		ImGui::SameLine(100);

		ImGui::Text(u8"HEIGHT: %.3f", m_spriteInfo.scale.y);

		//End Group Draw Sprite Info
		ImGui::EndGroup();
	}
	ImGui::Dummy(ImVec2{0, 5});
	//Draw Center Info
	ImGui::BeginGroup();
	{
		ImGui::Text(u8"CENTER INFO");

		ImGui::Text(u8"X OFFSET : ");
		ImGui::SameLine();
		ImGui::PushItemWidth(125);
		ImGui::InputFloat(u8"##X_OFFSET", &m_spriteInfo.offsetX, 0.1f);
		ImGui::PopItemWidth();

		ImGui::Text(u8"Y OFFSET : ");
		ImGui::SameLine();
		ImGui::PushItemWidth(125);
		ImGui::InputFloat(u8"##Y_OFFSET", &m_spriteInfo.offsetY, 0.1f);
		ImGui::PopItemWidth();

		//End Group Draw Center Info
		ImGui::EndGroup();
	}
	ImGui::Dummy(ImVec2{0, 5});
	//Draw Animation Info
	ImGui::BeginGroup();
	{
		static bool isReached = false;
		ImGui::Text(u8"ANIMATION INFO");

		ImGui::Text(u8"NAME : ");
		ImGui::SameLine(85);

		ImGui::PushItemWidth(125);
		ImGui::InputText(u8"##ANIMATION_NAME", &m_spriteInfo.name, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::PopItemWidth();

		ImGui::Text(u8"INDEX : %d", m_spriteInfo.index);

		ImGui::Text(u8"DURATION : ");
		ImGui::SameLine();
		ImGui::PushItemWidth(125);
		if (ImGui::InputFloat(u8"##ANIMATION_DURATION", &m_spriteInfo.duration, 0.01f, 1.f, "%.2f"))
		{
			m_spriteInfo.duration = (m_spriteInfo.duration <= 0.f) ? 0.f : m_spriteInfo.duration;
		}
		//0 이하 떨어지는 것을 방지
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2{0, 5});
		ImGui::Text(u8"BACKGROUND SIZE");
		ImGui::Text(u8"WIDTH : %d", static_cast<int>(m_spriteInfo.backGroundSize.x));
		ImGui::SameLine(100);
		ImGui::Text(u8"HEIGHT : %d", static_cast<int>(m_spriteInfo.backGroundSize.y));
		ImGui::Text(u8"PADDING : ");
		ImGui::SameLine();
		ImGui::PushItemWidth(125);
		if (ImGui::InputInt(u8"##LEAST_PADDING", &m_leastPadding, 1, 10))
		{
			//패딩값이 음수가 되는 것을 방지
			m_leastPadding = (m_leastPadding <= 0) ? 0 : m_leastPadding;

			ChangeCheckAllBackgroundSize();
		}
		ImGui::PopItemWidth();

		//Play&Stop Button Group
		ImGui::Dummy(ImVec2{});
		ImGui::SameLine(ImGui::GetWindowWidth() - 185);
		ImGui::BeginGroup();
		ImGui::Checkbox("REPEAT##ANIMATION", &m_repeat);
		ImGui::SameLine();
		if (ImGui::Button(u8"PLAY##ANIMATION")) { PlayPreview(); }
		ImGui::SameLine();
		if (ImGui::Button(u8"STOP##ANIMATION")) { StopPreview(); }
		ImGui::EndGroup();

		//Animation Modify Button Group
		ImGui::Dummy(ImVec2{});
		ImGui::SameLine(ImGui::GetWindowWidth() - 185);
		ImGui::BeginGroup();
		if (ImGui::Button(u8"APPEND")) { Append(); }
		ImGui::SameLine();
		if (ImGui::Button(u8"CHANGE")) { Modify(); }
		ImGui::SameLine();
		if (ImGui::Button(u8"REMOVE")) { Remove(); }
		ImGui::EndGroup();
		if (ImGui::Button("RESET")) { Reset(); }
		ImGui::SameLine();
		if (ImGui::Button("REVERSE")) { Reverse(); }
		//EndGroup Draw Animation Info
		ImGui::EndGroup();

		if (nullptr != m_pTarget)
		{
			if (ImGui::Button("APPLY TO GAME OBJECT"))
			{
				ApplyToTarget();
			}
			if (nullptr != m_pSelectedNode && nullptr == m_pSelectedNode->GetParent()->GetParent())
			{
				if (ImGui::Button("SET AS CURRENT ANIMATION"))
				{
					m_pTarget->Animator2D()->Play(ToWString(m_pSelectedNode->GetName()), true);
					if (nullptr != m_pTarget->MeshRender())
					{
						m_pTarget->MeshRender()->GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pTexture2D);
					}
				}
			}
		}
	}

	if (ImGui::Button("Split Texture"))
	{
		ImGui::OpenPopup("SPLIT POPUP");
	}
	static bool openPopUp = true;
	if (ImGui::BeginPopupModal("SPLIT POPUP", &openPopUp))
	{
		ImGui::Text("ANIMATION NAME");
		ImGui::SameLine();
		static std::string animationName{};
		ImGui::InputText("##ANIMATION_NAME_POPUP", &animationName);
		ImGui::Text("SLICE SIZE");
		ImGui::SameLine();
		static float sliceSize[2]{};
		ImGui::InputFloat2("##SLICE SIZE", sliceSize);
		ImGui::Text("STEP");
		ImGui::SameLine();
		static float step[2]{};
		ImGui::InputFloat2("##STEP", step);
		static int frameCount{};
		ImGui::InputInt("##FRAME_COUNT", &frameCount);
		if (ImGui::Button("OKAY"))
		{
			float fWidth  = m_pTexture2D->Width();
			float fHeight = m_pTexture2D->Height();

			// 픽셀 좌표를 0~1 UV 로 전환
			ImVec2 LT       = {0.f, 0.f};
			ImVec2 slice    = {sliceSize[0], sliceSize[1]};
			ImVec2 back     = slice;
			ImVec2 stepSize = {step[0], step[1]};
			// 프레임 정보 생성
			std::vector<SpriteInfo> vecInfo{};
			vecInfo.reserve(frameCount);
			for (int i = 0; i < frameCount; ++i)
			{
				SpriteInfo info{};

				info.position       = LT + (stepSize * static_cast<float>(i));
				info.scale          = slice;
				info.duration       = 0.1f;
				info.backGroundSize = back;
				info.index          = i;
				info.name           = animationName;
				info.offsetX        = info.scale.x * 0.5f;
				info.offsetY        = info.scale.y * 0.5f;
				vecInfo.push_back(info);
			}
			m_mapAnimations.insert({animationName, vecInfo});

			TreeRenew();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::EndChild();
}

void AnimationToolUI::ChangeCheckAllBackgroundSize()
{
	for (auto& [animationName, vecFrames] : m_mapAnimations)
	{
		std::transform(vecFrames.begin()
		             , vecFrames.end()
		             , vecFrames.begin()
		             , [this](SpriteInfo& spriteInfo)
		               {
			               spriteInfo.backGroundSize = ImVec2{m_leastBase, m_leastBase};
			               return spriteInfo;
		               });
	}
}

void AnimationToolUI::PlayPreview()
{
	m_isPlaying = true;

	PlayAnimReset();
}

void AnimationToolUI::StopPreview()
{
	m_isPlaying = false;

	PlayAnimReset();
}

void AnimationToolUI::UpdatePlayTime()
{
	if (false == m_isPlaying) return;
	if (m_bFinish) return;
	if (nullptr == m_pSelectedNode) return;

	m_fAccTime += DT;

	const auto iter = m_mapAnimations.find(m_spriteInfo.name);
	if (iter != m_mapAnimations.end())
	{
		const vector<SpriteInfo>& frames = iter->second;

		if (frames[m_iCurFrmIdx].duration < m_fAccTime)
		{
			m_fAccTime -= frames[m_iCurFrmIdx].duration;

			if (frames.size() - 1 <= m_iCurFrmIdx)
			{
				m_bFinish = true;
				if (m_repeat)
				{
					PlayAnimReset();
				}
			}
			else
			{
				++m_iCurFrmIdx;
			}
		}
	}
}


void AnimationToolUI::TreeRenew()
{
	m_pSelectedNode = nullptr;
	m_pAnimTree->Clear();

	//size가 0인 애니메이션은 모두 제거
	for (auto iter = m_mapAnimations.begin(); iter != m_mapAnimations.end();)
	{
		if (iter->second.size() == 0)
		{
			iter = m_mapAnimations.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// 애니메이션을 Tree 에 표시
	for (auto& [animationName, vecFrames] : m_mapAnimations)
	{
		TreeNode* pAnimNode = m_pAnimTree->AddTreeNode(nullptr, animationName, &vecFrames);

		int FrameNum = 0;
		for (SpriteInfo& spriteInfo : vecFrames)
		{
			m_pAnimTree->AddTreeNode(pAnimNode, std::to_string(FrameNum++), &spriteInfo);
		}
	}
}

void AnimationToolUI::ItemClicked(void* pSelectedTreeNode)
{
	TreeNode* pNode = static_cast<TreeNode*>(pSelectedTreeNode);
	m_pSelectedNode = pNode;

	const TreeNode* pDummy = pNode->GetParent();
	if (nullptr != pDummy->GetParent())//프레임 노드
	{
		m_spriteInfo = *static_cast<SpriteInfo*>(pNode->GetData());
		m_startPos   = m_spriteInfo.position;
		m_lastPos    = m_spriteInfo.scale + m_startPos;
	}
}

void AnimationToolUI::Append()
{
	if (0 >= m_spriteInfo.name.size()) return;

	// 애니메이션이 존재하는지 검사
	const auto iter = m_mapAnimations.find(m_spriteInfo.name);
	// 없는 경우
	if (iter == m_mapAnimations.end())
	{
		vector<SpriteInfo> frames{};
		m_spriteInfo.index = 0;
		frames.push_back(m_spriteInfo);
		m_mapAnimations.insert({m_spriteInfo.name, std::move(frames)});
	}
	else // 기존에 존재하는 경우
	{
		vector<SpriteInfo>& frames = iter->second;
		m_spriteInfo.index         = static_cast<int>(frames.size());
		frames.push_back(m_spriteInfo);
	}
	TreeRenew();
}

void AnimationToolUI::Modify()
{
	if (nullptr == m_pSelectedNode) return;

	// 프레임의 이름을 가져와서 찾아옴
	auto iter = m_mapAnimations.find(m_spriteInfo.name);
	// 프레임을 수정하는 경우(프레임 전체 데이터), 만약 애니메이션의 이름을 변경하는 것을 목적으로 한다면 해당 애니메이션 이름을 찾을 수 없다.
	if (iter != m_mapAnimations.end())
	{
		iter->second[m_spriteInfo.index] = m_spriteInfo;
	}
	// 애니메이션을 수정하려는 경우(이름만 변경 가능)
	else
	{
		// 선택된 노드가 애니메이션인 경우
		const TreeNode* pDummy = m_pSelectedNode->GetParent();
		//더미의 parent가 nullptr이므로, 우리가 선택한 노드가 애니메이션 노드임을 뜻함
		if (nullptr == pDummy->GetParent())
		{
			// 선택된 노드의 이름(애니메이션 이름)을 기반으로 애니메이션 pair를 찾아온다.
			auto iter = m_mapAnimations.find(m_pSelectedNode->GetName());
			if (iter != m_mapAnimations.end())
			{
				for (auto& spriteInfo : iter->second)
				{
					spriteInfo.name = m_spriteInfo.name;
				}

				auto vecSpriteInfo = std::move(m_mapAnimations[iter->first]);
				
				m_mapAnimations.erase(iter);
				m_mapAnimations.insert({m_spriteInfo.name, std::move(vecSpriteInfo)});

				m_pSelectedNode = nullptr;
			}
		}
	}

	TreeRenew();
}

void AnimationToolUI::Remove()
{
	if (nullptr == m_pSelectedNode) return;

	// 선택된 노드가 애니메이션인 경우
	const TreeNode* pDummy = m_pSelectedNode->GetParent();
	//더미의 parent가 nullptr이므로, 우리가 선택한 노드가 애니메이션 노드임을 뜻함
	if (nullptr == pDummy->GetParent())
	{
		m_mapAnimations.erase(m_pSelectedNode->GetName());
		m_pSelectedNode = nullptr;
	}
	// 선택된 노드가 프레임인 경우
	else
	{
		// pair를 찾는다.
		TreeNode* pAnimationNode = m_pSelectedNode->GetParent();

		auto iter = m_mapAnimations.find(pAnimationNode->GetName());
		if (iter != m_mapAnimations.end())
		{
			//animation frame들이 인덱스 순으로 정렬되어 있고, 이 순서는 내부 frame vector에서도 동일한 인덱스
			const SpriteInfo* pSelectedFrame = static_cast<SpriteInfo*>(m_pSelectedNode->GetData());
			iter->second.erase(iter->second.begin() + pSelectedFrame->index);

			auto& vecFrames = iter->second;
			std::transform(vecFrames.begin()
			             , vecFrames.end()
			             , vecFrames.begin()
			             , [i = 0](SpriteInfo& info)mutable
			               {
				               info.index = i++;

				               return info;
			               });

			m_pSelectedNode = nullptr;
		}
	}
	TreeRenew();
}

void AnimationToolUI::Reverse()
{
	if (nullptr == m_pSelectedNode) return;

	// 선택된 노드가 애니메이션인 경우
	const TreeNode* pDummy = m_pSelectedNode->GetParent();
	//더미의 parent가 nullptr이므로, 우리가 선택한 노드가 애니메이션 노드임을 뜻함
	if (nullptr == pDummy->GetParent())
	{
		auto iter = m_mapAnimations.find(m_pSelectedNode->GetName());
		assert(iter!=m_mapAnimations.end());
		auto& vecSpriteInfo = iter->second;

		std::reverse(vecSpriteInfo.begin(), vecSpriteInfo.end());
		std::transform(vecSpriteInfo.begin()
		             , vecSpriteInfo.end()
		             , vecSpriteInfo.begin()
		             , [i=0](SpriteInfo& info) mutable
			               {
				               info.index = i++;
				               return info;
			               });

		TreeRenew();
	}
}

void AnimationToolUI::DrawFrames()
{
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 4;
	const float  width      = windowSize.x * .5f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (1.f / 3.f);
	ImGui::BeginChild(u8"ANIMATION_FRAME_SCREEN", ImVec2{width, height}, true, ImGuiWindowFlags_HorizontalScrollbar);

	// 선택된 노드가 있는 경우
	if (nullptr != m_pSelectedNode)
	{
		std::map<std::string, std::vector<SpriteInfo>>::iterator iter{};

		const TreeNode* pDummy = m_pSelectedNode->GetParent();
		//우리가 선택한 노드가 프레임 노드
		if (nullptr != pDummy->GetParent())
		{
			const SpriteInfo* pSpriteInfo = static_cast<SpriteInfo*>(m_pSelectedNode->GetData());
			iter                          = m_mapAnimations.find(pSpriteInfo->name);
			assert(iter != m_mapAnimations.end());
		}
		//더미의 parent가 nullptr이면 , 우리가 선택한 노드가 애니메이션 노드
		else if (nullptr == pDummy->GetParent())
		{
			//현재 선택된 노드의 이름은 애니메이션의 이름이다
			iter = m_mapAnimations.find(m_pSelectedNode->GetName());
			assert(iter != m_mapAnimations.end());
		}
		const auto&       vecFrames           = iter->second;
		ImDrawList* const pDrawList           = ImGui::GetWindowDrawList();
		const ImVec2      drawFramesWindowPos = ImGui::GetWindowPos();

		const ImVec2 backgroundSize = {m_leastBase, m_leastBase};
		//DrawFrames 윈도우 크기의 y축이 더 짧으므로 여기선 y축을 기준
		const float baseSize = ImGui::GetWindowSize().y;
		//DrawFrames 윈도우 크기의 절반만큼의 크기로 보이도록 비율을 구한다.
		const float ratio = baseSize / backgroundSize.x * 0.5f;


		for (size_t i = 0; i < vecFrames.size(); ++i)
		{
			ImVec2 offset{backgroundSize.x * i * ratio, baseSize * 0.2f};
			ImVec2 backgroundPos = drawFramesWindowPos + offset;
			//스크롤 시에, 윈도우가 이동하는 만큼을 빼줘서 화면에 보이도록 한다.
			ImGui::SetNextWindowPos(backgroundPos - ImVec2{ImGui::GetScrollX(), 0});

			//각 이미지가 표시될 윈도우 생성
			ImGui::BeginChild(std::string("BACKGROUND" + std::to_string(i)).c_str()
			                , backgroundSize * ratio
			                , false
			                , ImGuiWindowFlags_NoScrollbar);

			//백그라운드 표시(윈도우의 크기와 같다)
			pDrawList->AddRect(ImGui::GetWindowPos()
			                 , ImGui::GetWindowPos() + ImGui::GetWindowSize()
			                 , IM_COL32(255, 0, 0, 255));

			const ImVec2 sliceOffset = backgroundSize * 0.5f - vecFrames[i].scale * 0.5f;
			const ImVec2 slicePos    = sliceOffset * ratio;
			ImGui::SetCursorPos(slicePos);

			const ImVec2 startUV               = vecFrames[i].position / m_pTexture2D->GetSize();
			const ImVec2 endUV                 = (vecFrames[i].position + vecFrames[i].scale) / m_pTexture2D->GetSize();
			const ImVec2 backGroundLeftTop     = vecFrames[i].position - sliceOffset;
			const ImVec2 backGroundRightBottom = backGroundLeftTop + vecFrames[i].backGroundSize;

			ImGui::PushID(vecFrames[i].index);
			if (ImGui::ImageButton(m_pTexture2D->GetSRV().Get()
			                     , vecFrames[i].scale * ratio
			                     , startUV
			                     , endUV
			                     , 0))
			{
				m_spriteInfo            = vecFrames[i];
				m_backgroundLeftTop     = backGroundLeftTop;
				m_backgroundRightBottom = backGroundRightBottom;
				m_startPos              = vecFrames[i].position;
				m_lastPos               = vecFrames[i].position + vecFrames[i].scale;
			}
			ImGui::PopID();
			//BACKGROUND WINDOW END
			ImGui::EndChild();

			ImGui::SameLine();
		}
	}
	//DRAW FRAME WINDOW END
	ImGui::EndChild();
}

void AnimationToolUI::DrawModifyScreen()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 2;
	const float  width      = windowSize.x * 0.25f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (1.f / 3.f);

	const float baseSize = std::min(width, height);

	ImGui::BeginChild(u8"ANIMATION_MODIFY_SCREEN", ImVec2{width, height}, true, ImGuiWindowFlags_NoScrollbar);

	if (nullptr != m_pTexture2D && false == m_isDrawing)
	{
		const float ratio = baseSize / m_spriteInfo.backGroundSize.x;

		m_spriteInfo.position = m_startPos;
		m_spriteInfo.scale    = m_lastPos - m_startPos;

		const ImVec2 startUV      = m_backgroundLeftTop / m_pTexture2D->GetSize();
		const ImVec2 endUV        = m_backgroundRightBottom / m_pTexture2D->GetSize();
		const ImVec2 DrawStartPos = (ImGui::GetContentRegionAvail() - m_spriteInfo.backGroundSize * ratio) * 0.5f
		                            + ImVec2{
			                            (m_spriteInfo.offsetX)
			                          , -m_spriteInfo.offsetY
		                            } * ratio;
		ImGui::SetCursorPos(DrawStartPos);


		//ImGui::Image(m_pTexture2D->GetSRV().Get(), m_spriteInfo.backGroundSize * ratio, startUV, endUV);
		ImGui::Image(m_pTexture2D->GetSRV().Get()
		           , m_spriteInfo.backGroundSize * ratio
		           , startUV
		           , endUV);

		const ImVec2 modifyScreenSize = ImGui::GetWindowSize();
		const ImVec2 windowPos        = ImGui::GetWindowPos();
		ImDrawList*  pDrawList        = ImGui::GetWindowDrawList();

		pDrawList->AddLine(ImVec2{0.f, modifyScreenSize.y * 0.5f} + windowPos
		                 , ImVec2{modifyScreenSize.x, modifyScreenSize.y * 0.5f} + windowPos
		                 , IM_COL32(255, 0, 0, 255)
		                 , 1);
		pDrawList->AddLine(ImVec2{modifyScreenSize.x * 0.5f, 0.f} + windowPos
		                 , ImVec2{modifyScreenSize.x * 0.5f, modifyScreenSize.y} + windowPos
		                 , IM_COL32(255, 0, 0, 255)
		                 , 1);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}

void AnimationToolUI::DrawPreviewScreen()
{
	// 미리보기 화면을 그리기 전 애니메이션 재생 시간을 업데이트
	UpdatePlayTime();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 2;
	const float  width      = windowSize.x * 0.25f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING) * (1.f / 3.f);

	const float baseSize = std::min(width, height);

	ImGui::BeginChild(u8"ANIMATION_PREVIEW_SCREEN", ImVec2{width, height}, true, ImGuiWindowFlags_None);

	if (nullptr != m_pTexture2D
	    && false == m_isDrawing
	    && nullptr != m_pSelectedNode)
	{
		std::map<std::string, std::vector<SpriteInfo>>::iterator iter{};

		const TreeNode* pDummy = m_pSelectedNode->GetParent();
		//우리가 선택한 노드가 프레임 노드
		if (nullptr != pDummy->GetParent())
		{
			const SpriteInfo* pSpriteInfo = static_cast<SpriteInfo*>(m_pSelectedNode->GetData());
			iter                          = m_mapAnimations.find(pSpriteInfo->name);
			assert(iter != m_mapAnimations.end());
			if (false == m_isPlaying)
				m_iCurFrmIdx = pSpriteInfo->index;
		}
		//더미의 parent가 nullptr이면 , 우리가 선택한 노드가 애니메이션 노드
		else if (nullptr == pDummy->GetParent())
		{
			//현재 선택된 노드의 이름은 애니메이션의 이름이다
			iter = m_mapAnimations.find(m_pSelectedNode->GetName());
			assert(iter != m_mapAnimations.end());
			m_iCurFrmIdx = 0;
		}

		if (m_iCurFrmIdx >= iter->second.size())
		{
			m_iCurFrmIdx = 0;
		}

		const SpriteInfo frame   = iter->second[m_iCurFrmIdx];
		//const float      ratio   = baseSize / frame.backGroundSize.x;
		const float  ratio   = baseSize / frame.backGroundSize.x;
		const ImVec2 startUV = frame.position / m_pTexture2D->GetSize();
		const ImVec2 endUV   = (frame.position + frame.scale) / m_pTexture2D->GetSize();

		const ImVec2 DrawStartPos = (ImGui::GetContentRegionAvail() - frame.scale * ratio) * 0.5f
		                            + ImVec2{
			                            (m_spriteInfo.offsetX)
			                          , -m_spriteInfo.offsetY
		                            } * ratio;
		ImGui::SetCursorPos(DrawStartPos);
		ImDrawList*  pDrawList         = ImGui::GetWindowDrawList();
		const ImVec2 previewWindowSize = ImGui::GetWindowSize();
		const ImVec2 windowPos         = ImGui::GetWindowPos();

		ImGui::Image(m_pTexture2D->GetSRV().Get(), frame.scale * ratio, startUV, endUV);

		pDrawList->AddLine(ImVec2{0.f, previewWindowSize.y * 0.5f} + windowPos
		                 , ImVec2{previewWindowSize.x, previewWindowSize.y * 0.5f} + windowPos
		                 , IM_COL32(255, 0, 0, 255)
		                 , 1);
		pDrawList->AddLine(ImVec2{previewWindowSize.x * 0.5f, 0.f} + windowPos
		                 , ImVec2{previewWindowSize.x * 0.5f, previewWindowSize.y} + windowPos
		                 , IM_COL32(255, 0, 0, 255)
		                 , ratio);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}
