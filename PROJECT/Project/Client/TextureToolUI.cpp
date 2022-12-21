#include "pch.h"
#include "TextureToolUI.h"

#include <Engine/CCore.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CGameObject.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <Engine/CMeshRender.h>

#include "ImGui/imgui_internal.h"
#undef max

namespace
{
	constexpr int INVALID_ITEM_INDEX = -1;
}
TextureToolUI::TextureToolUI()
	:
	UI{ "TextureTool" }
	, m_pTarget{ nullptr }
	, m_leastBase{ 0 }
	, m_zoomScale{ 1.f }
	, m_isSelected{ false }
	, m_isOpenWarningPopUP{ false }
	, m_hasMetaInfo{ false } {}

TextureToolUI::~TextureToolUI() = default;

void TextureToolUI::render_update()
{
	DrawMenu();
	DrawImage();
	ImGui::SameLine();
	DrawSelectedImageInfo();
}

void TextureToolUI::SetTarget(CGameObject* pGameObject)
{
	m_pTarget = pGameObject;

	if (nullptr != m_pTarget)
	{
		Ptr<CMaterial> pMaterial = m_pTarget->MeshRender()->GetMaterial();
		m_pTexture2D = pMaterial->GetTexParam(TEX_PARAM::TEX_0);
		if (nullptr != m_pTexture2D)
		{
			std::wstring metaFileName = m_pTexture2D->GetKey();
			metaFileName += L".txt";
			const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
			m_filePath = contentPath + L"meta\\" + metaFileName;
			if (false == std::filesystem::exists(m_filePath)) { return; }
			LoadMetaInfo();

			const Vec2 textureSize = m_pTexture2D->GetSize();

			m_selectedImageInfo.name = ToString(m_pTarget->MeshRender()->GetSpriteName());

			const Vec2 leftTop = *(Vec2*)pMaterial->GetScalarParam(SCALAR_PARAM::VEC2_0);
			m_selectedImageInfo.leftTop = leftTop * textureSize;

			const Vec2 scale = *(Vec2*)pMaterial->GetScalarParam(SCALAR_PARAM::VEC2_1);
			m_selectedImageInfo.scale = scale * textureSize;

			const Vec2 backgroundSize = *(Vec2*)pMaterial->GetScalarParam(SCALAR_PARAM::VEC2_2);
			m_selectedImageInfo.backgroundSize = backgroundSize * textureSize;
		}
	}
}

void TextureToolUI::OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath)
{
	wchar_t      szName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = filterFormat;
	ofn.nFilterIndex = filterIndex;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;

	std::wstring filePath = CPathMgr::GetInst()->GetContentPath();
	filePath += relativePath.c_str();
	ofn.lpstrInitialDir = filePath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetSaveFileName(&ofn))
	{
		m_filePath = ofn.lpstrFile;
	}
}

Ptr<CTexture> TextureToolUI::LoadTexture() const
{
	std::wstring extension = std::filesystem::path{ m_filePath }.extension();
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

	const std::wstring fileName = std::filesystem::path{ m_filePath }.filename();
	const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	const std::wstring relativePath = L"texture\\" + fileName;
	hr = SaveToWICFile(image.GetImages()
		, image.GetImageCount()
		, WIC_FLAGS_NONE
		, GetWICCodec(WIC_CODEC_PNG)
		, std::wstring{ contentPath + relativePath }.c_str());
	assert(S_OK == hr);
	Ptr<CTexture> pTexture = CResMgr::GetInst()->Load<CTexture>(relativePath, relativePath, false);

	return pTexture;
}

void TextureToolUI::ApplyToTarget()
{
	//값을 넘겨줄 때는 0~1 사이로 변경해서 넘겨줘야 한다.
	CMeshRender* pMeshRender = m_pTarget->MeshRender();

	pMeshRender->SetSpriteName(ToWString(m_selectedImageInfo.name));

	pMeshRender->SetTexture(m_pTexture2D);
	pMeshRender->SetHasMetaInfo(true);

	const Vec2 leftTop = Vec2{ m_selectedImageInfo.leftTop.x, m_selectedImageInfo.leftTop.y } / m_pTexture2D->GetSize();
	pMeshRender->SetLeftTop(leftTop);

	const Vec2 scale = Vec2{ m_selectedImageInfo.scale.x, m_selectedImageInfo.scale.y } / m_pTexture2D->GetSize();
	pMeshRender->SetScale(scale);

	const Vec2 backgroundSize = Vec2{ m_selectedImageInfo.backgroundSize.x, m_selectedImageInfo.backgroundSize.y } /
		m_pTexture2D->GetSize();
	pMeshRender->SetBackgroundSize(backgroundSize);

	Reset();

	Deactivate();
}

void TextureToolUI::GetTexture()
{
	//Find Image
	OpenFileDialog(TEXTURE_FILTER_FORMAT, 0, L"texture");
	if (m_filePath.empty()) return;

	//1. 해당 파일이 contentPath 내의 texture 폴더 밑에 존재하는지 확인
	const std::wstring fileName = std::filesystem::path{ m_filePath }.filename();
	const std::wstring relativePath = L"texture\\" + fileName;
	m_pTexture2D = CResMgr::GetInst()->FindRes<CTexture>(relativePath);
	//만약 존재하지 않는다면, 해당 텍스처를 로딩 후 texture폴더 밑으로 복사
	if (nullptr == m_pTexture2D)
	{
		m_pTexture2D = LoadTexture();
	}
}

void TextureToolUI::LoadMetaInfo()
{
	if (m_filePath.empty()) { return; }


	std::ifstream inputFile{ m_filePath };
	if (inputFile.is_open())
	{
		m_hasMetaInfo = true;

		std::string textInFile{};
		inputFile.seekg(0, std::ios::end);
		const size_t size = inputFile.tellg();
		textInFile.resize(size);
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(textInFile.data(), size);

		std::stringstream ss{ textInFile };
		std::string       line{};

		ImageInfo info{};
		while (std::getline(ss, line, '\n'))
		{
			if (nullptr == m_pTexture2D && line.find(".png") != line.npos)
			{
				std::wstring filePath = L"texture\\" + ToWString(line);
				m_pTexture2D = CResMgr::GetInst()->Load<CTexture>(filePath, filePath, false);
				assert(nullptr != m_pTexture2D);
			}
			else if ("SPRITE NAME" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				if (line.empty()) { continue; }
				info.name = line;
			}
			else if ("X" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.leftTop.x = std::stof(line);
			}
			else if ("Y" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.leftTop.y = std::stof(line);
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
				info.offset.x = (info.scale.x * (std::stof(line) - 0.5f));
			}
			else if ("OFFSET Y" == line)
			{
				//한 번 전진
				std::getline(ss, line, '\n');
				info.offset.y = (info.scale.y * (std::stof(line) - 0.5f));

				//스프라이트 하나의 정보가 가져왔으므로, 이것을 가공하여 트리에 출력
				float spriteBackgroundLength = std::max(info.scale.x, info.scale.y);
				m_leastBase = (spriteBackgroundLength > m_leastBase) ? spriteBackgroundLength : m_leastBase;
				m_vecImageInfo.push_back(info);
			}
		}

		//Background 갱신
		for (auto& imageInfo : m_vecImageInfo)
		{
			imageInfo.backgroundSize = ImVec2{ m_leastBase, m_leastBase };
		}
	}
	else
	{
		assert(nullptr);
	}
}

void TextureToolUI::SetZoomScale()
{
	//const ImVec2   canvasLeftTop    = ImGui::GetCursorScreenPos();
	const ImGuiIO& io = ImGui::GetIO();
	//const ImVec2   mousePosInCanvas = io.MousePos - canvasLeftTop;

	// 마우스 휠 => 크기 조정
	if (KEY_PRESSED(KEY::LCTRL))
	{
		// Minimum zoom = x0.1 (near camera limit)
		// Maximum zoom = x5.0 (it can fit an image with 1600px width)
		m_zoomScale = m_zoomScale + (io.MouseWheel * 0.3f);
		m_zoomScale = std::clamp(m_zoomScale, 0.1f, 5.0f);
	}
}

void TextureToolUI::Reset()
{
	m_pTexture2D = {};
	m_selectedImageInfo = {};
	m_vecImageInfo.clear();
	m_isSelected = false;
	m_leastBase = 0.f;
	m_hasMetaInfo = false;
}

void TextureToolUI::DrawMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"Menu"))
		{
			if (ImGui::MenuItem(u8"Load Texture")) { GetTexture(); }
			if (ImGui::MenuItem(u8"Load MetaInfo"))
			{
				OpenFileDialog(TXT_FILTER_FORMAT, 0, L"meta");
				if (m_filePath.empty()) { return; }
				LoadMetaInfo();
			}
			ImGui::EndMenu();
		}
		//End MenuBar
		ImGui::EndMenuBar();
	}
}

void TextureToolUI::DrawImage()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding = ImGui::GetStyle().FramePadding.x * 4;
	const float  width = windowSize.x * 0.8f - padding;
	const float  height = (windowSize.y - HEIGHT_PADDING);
	ImGui::BeginChild(u8"TEXTURE_IMAGE_SCREEN"
		, ImVec2{ width, height }
		, true
		, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

	if (nullptr != m_pTexture2D)
	{
		ImGui::Image(m_pTexture2D->GetSRV().Get(), m_pTexture2D->GetSize() * m_zoomScale);
		if (true == m_hasMetaInfo)
		{
			DrawImageButtonOnSprite();
		}
		else
		{
			DrawImageAsWholeSprite();
		}

		SetZoomScale();
	}
	ImGui::EndChild();

	ImGui::PopStyleVar(3);
}

void TextureToolUI::DrawImageButtonOnSprite()
{
	//1. 모든 ImageInfo를 순회하면서 해당 이미지 위에 Rect를 그리도록 한다.
	// 마우스 상태 저장 변수들
	const ImVec2 mousePosInImage = ImGui::GetMousePos();
	int          hoverItemIndex = INVALID_ITEM_INDEX;
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	for (int i = 0; i < m_vecImageInfo.size(); ++i)
	{
		//스크롤 시에, 윈도우가 이동하는 만큼을 빼줘서 화면에 보이도록 한다.
		const ImVec2 leftTop = ImGui::GetWindowPos() + (m_vecImageInfo[i].leftTop) * m_zoomScale
			- ImVec2{
				ImGui::GetScrollX()
			  , ImGui::GetScrollY()
		};
		const ImVec2 rightBottom = ImGui::GetWindowPos() + (m_vecImageInfo[i].leftTop + m_vecImageInfo[i].scale) *
			m_zoomScale
			- ImVec2{
				ImGui::GetScrollX()
			  , ImGui::GetScrollY()
		};
		// 마우스 위치가 canvas안 이라면
		if (leftTop.x < mousePosInImage.x && mousePosInImage.x < rightBottom.x
			&& leftTop.y < mousePosInImage.y && mousePosInImage.y < rightBottom.y)
		{
			hoverItemIndex = i;
			pDrawList->AddRectFilled(leftTop, rightBottom, IM_COL32(128, 128, 128, 128));
			pDrawList->AddRect(leftTop, rightBottom, IM_COL32(0, 255, 0, 255));
		}
		else
		{
			pDrawList->AddRect(leftTop, rightBottom, IM_COL32(0, 0, 255, 255));
		}
	}

	// 캔버스가 사용자의 마우스 입력을 감지하도록 버튼 추가
	ImGui::InvisibleButton("TEXTURE_CANVAS", m_pTexture2D->GetSize() * m_zoomScale, ImGuiButtonFlags_MouseButtonLeft);


	// 마우스 동작
	if (INVALID_ITEM_INDEX != hoverItemIndex
		&& ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_selectedImageInfo = m_vecImageInfo[hoverItemIndex];
		m_isSelected = true;
	}
}

void TextureToolUI::DrawImageAsWholeSprite()
{
	const ImVec2 mousePosInImage = ImGui::GetMousePos();
	bool         isHovered = false;
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	//스크롤 시에, 윈도우가 이동하는 만큼을 빼줘서 화면에 보이도록 한다.
	const ImVec2 leftTop = ImGui::GetWindowPos() - ImVec2{ ImGui::GetScrollX(), ImGui::GetScrollY() };
	const ImVec2 rightBottom = ImGui::GetWindowPos() + m_pTexture2D->GetSize() * m_zoomScale
		- ImVec2{
			ImGui::GetScrollX()
		  , ImGui::GetScrollY()
	};
	// 마우스 위치가 canvas안 이라면
	if (leftTop.x < mousePosInImage.x && mousePosInImage.x < rightBottom.x
		&& leftTop.y < mousePosInImage.y && mousePosInImage.y < rightBottom.y)
	{
		isHovered = true;
		pDrawList->AddRectFilled(leftTop, rightBottom, IM_COL32(128, 128, 128, 128));
		pDrawList->AddRect(leftTop, rightBottom, IM_COL32(0, 255, 0, 255));
	}
	else
	{
		pDrawList->AddRect(leftTop, rightBottom, IM_COL32(0, 0, 255, 255));
	}


	// 캔버스가 사용자의 마우스 입력을 감지하도록 버튼 추가
	ImGui::InvisibleButton("TEXTURE_CANVAS", m_pTexture2D->GetSize() * m_zoomScale, ImGuiButtonFlags_MouseButtonLeft);


	// 마우스 동작
	if (true == isHovered
		&& ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_selectedImageInfo.backgroundSize = m_pTexture2D->GetSize();
		m_selectedImageInfo.leftTop = ImVec2{ 0, 0 };

		std::wstring key = m_pTexture2D->GetKey();
		const size_t pos = key.find_last_of(L"\\");
		assert(pos != key.npos);
		key = key.substr(pos + 1);
		key = key.substr(0, key.size() - 4);
		m_selectedImageInfo.name = ToString(key);
		m_selectedImageInfo.offset = ImVec2{ 0, 0 };
		m_selectedImageInfo.scale = m_pTexture2D->GetSize();
		m_isSelected = true;
	}
}

void TextureToolUI::DrawSelectedImageInfo()
{
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding = ImGui::GetStyle().FramePadding.x * 2;
	const float  width = windowSize.x * 0.2f - padding;
	const float  height = (windowSize.y - HEIGHT_PADDING);
	ImGui::BeginChild(u8"TEXTURE_INSPECTOR_SCREEN", ImVec2{ width, height }, true);

	//Draw Sprite Info
	ImGui::BeginGroup();
	{
		ImGui::Text(u8"SPRITE INFO");
		ImGui::Text("Name : %s", m_selectedImageInfo.name.c_str());
		ImGui::Text(u8"X : %.3f", m_selectedImageInfo.leftTop.x);
		ImGui::SameLine(100);

		ImGui::Text(u8"WIDTH : %.3f", m_selectedImageInfo.scale.x);

		ImGui::Text(u8"Y : %.3f", m_selectedImageInfo.leftTop.y);
		ImGui::SameLine(100);

		ImGui::Text(u8"HEIGHT: %.3f", m_selectedImageInfo.scale.y);

		ImGui::Text(u8"Background Size : X : %.3f, Y : %.3f"
			, m_selectedImageInfo.backgroundSize.x
			, m_selectedImageInfo.backgroundSize.y);
		//End Group Draw Sprite Info
		ImGui::EndGroup();
	}

	if (ImGui::Button(u8"Reset")) { Reset(); }

	if (nullptr != m_pTarget)
	{
		if (ImGui::Button("APPLY TO GAME OBJECT"))
		{
			if (true == m_isSelected)
			{
				ApplyToTarget();
			}
			else
			{
				m_isOpenWarningPopUP = true;
			}
		}
	}

	if (true == m_isOpenWarningPopUP)
	{
		ImGui::OpenPopup("WARNING_POPUP");
	}
	if (ImGui::BeginPopup("WARNING_POPUP"))
	{
		ImGui::Text("NO SELECTED SPRITE. SELECT SPRITE FIRST!");
		if (ImGui::Button("Close"))
		{
			m_isOpenWarningPopUP = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::EndChild();
}
