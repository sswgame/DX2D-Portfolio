#include "pch.h"
#include "TileMapToolUI.h"
#include <Engine/CCore.h>
#include "ImGui/imgui_stdlib.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <fstream>
#include <sstream>

#include "ImGui/imgui_internal.h"
#include <CMarkUp/Markup.h>
#include <Engine/CGameObject.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CTileMap.h>

namespace
{
	constexpr int INVALID_TILE_INDEX = -1;
}

TileMapToolUI::TileMapToolUI()
	:
	UI{"TileMapTool"}
  , m_pTarget{nullptr}
  , m_tileCountX{10}
  , m_tileCountY{10}
  , m_rowCount{0}
  , m_colCount{0}
  , m_vSlicePixel{16, 16}
  , m_zoomScale{1.f}
  , m_openTilePallet{false}
  , m_prevCountY{0}
  , m_selectedTileIndex{INVALID_TILE_INDEX}
  , m_openLeft{false}
  , m_openRight{false}
  , m_openTop{false}
  , m_openBottom{false}
  , m_isFailed{false}
  , m_hasLeft{false}
  , m_hasRight{false}
  , m_hasTop{false}
  , m_hasBottom{false}
  , m_imageIndex{0}
{
	m_vecGridCurrentTileID.resize(m_tileCountY);
	m_vecTileData.resize(m_tileCountY);

	for (size_t i = 0; i < m_tileCountY; ++i)
	{
		m_vecGridCurrentTileID[i].resize(m_tileCountX);
		m_vecTileData[i].resize(m_tileCountX);
	}

	m_prevCountY = m_tileCountY;
}

TileMapToolUI::~TileMapToolUI() = default;

void TileMapToolUI::render_update()
{
	DrawMenu();
	DrawTile();
	ImGui::SameLine();
	DrawTileInspector();
}

void TileMapToolUI::SetTarget(CGameObject* pTarget)
{
	m_pTarget = pTarget;
	if (nullptr != m_pTarget)
	{
		const CTileMap* pTileMap = m_pTarget->TileMap();
		m_pTexture2D = pTileMap->GetTexture();
		m_vSlicePixel = (pTileMap->GetTileSize() != Vec2{0, 0}) ? pTileMap->GetTileSize() : Vec2{16, 16};
		m_rowCount = (nullptr != m_pTexture2D) ? static_cast<int>(m_pTexture2D->Height() / m_vSlicePixel.y) : 0;
		m_colCount = (nullptr != m_pTexture2D) ? static_cast<int>(m_pTexture2D->Width() / m_vSlicePixel.x) : 0;
		m_tileCountX = (0 < pTileMap->GetTileDatas().size()) ? pTileMap->GetTileCountX() : 10;
		m_tileCountY = (0 < pTileMap->GetTileDatas().size()) ? pTileMap->GetTileCountY() : 10;

		m_vecTileData.resize(m_tileCountY);
		for (int i = 0; i < m_tileCountY; ++i)
		{
			m_vecTileData[i].resize(m_tileCountX);
		}

		for (int i = 0; i < m_tileCountY; ++i)
		{
			for (int j = 0; j < m_tileCountX; ++j)
			{
				if (pTileMap->GetTileDatas().size())
				{
					m_vecTileData[i][j] = pTileMap->GetTileDatas()[j + i * m_tileCountX];
				}
				else
				{
					m_vecTileData[i][j] = {};
				}
			}
		}
	}
}

void TileMapToolUI::OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath)
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

void TileMapToolUI::LoadTexture()
{
	OpenFileDialog(TEXTURE_FILTER_FORMAT, 0, L"texture");
	if (m_filePath.empty()) return;

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

	hr = SaveToWICFile(image.GetImages()
	                 , image.GetImageCount()
	                 , WIC_FLAGS_NONE
	                 , GetWICCodec(WIC_CODEC_PNG)
	                 , std::wstring{contentPath + relativePath}.c_str());
	assert(S_OK == hr);

	m_pTexture2D = CResMgr::GetInst()->Load<CTexture>(relativePath, relativePath, false);
	m_colCount   = static_cast<int>(m_pTexture2D->Width() / m_vSlicePixel.x);
	m_rowCount   = static_cast<int>(m_pTexture2D->Height() / m_vSlicePixel.y);
}

void TileMapToolUI::DrawMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"Menu"))
		{
			if (ImGui::MenuItem(u8"New")) { New(); }
			if (ImGui::BeginMenu(u8"Load"))
			{
				if (ImGui::MenuItem(u8"Load Texture")) { LoadTexture(); }
				if (ImGui::MenuItem(u8"Load RoomInfo")) { RoomMetaFromXML(); }
				if (ImGui::MenuItem(u8"Load TileMeta Info")) { LoadTileMetaInfo(); }
				if (ImGui::MenuItem("Load .tile")) { LoadTileFile(); }
				//if (ImGui::MenuItem(u8"Load Map(Version 2)")) { LoadMap(); }
				//EndMenu
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(u8"Save")) { Save(); }
			//EndMenu
			ImGui::EndMenu();
		}
		//End MenuBar
		ImGui::EndMenuBar();
	}
}

void TileMapToolUI::DrawTile()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
	//Show Image to the left side of the window
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 4;
	const float  width      = windowSize.x * 0.75f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING);
	ImGui::BeginChild(u8"TILE_IMAGE_SCREEN"
	                , ImVec2{width, height}
	                , true
	                , ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

	if (0 < m_tileCountX && 0 < m_tileCountY && 0 < m_vSlicePixel.x && 0 < m_vSlicePixel.y)
	{
		DrawCanvasAndGrid();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}

void TileMapToolUI::DrawTileInspector()
{
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const float  padding    = ImGui::GetStyle().FramePadding.x * 2;
	const float  width      = windowSize.x * 0.25f - padding;
	const float  height     = (windowSize.y - HEIGHT_PADDING);
	ImGui::BeginChild(u8"TILE_INSPECTOR_SCREEN", ImVec2{width, height}, true);

	//TileMap Info
	ImGui::BeginGroup();
	{
		ImGui::Text(u8"TILE MAP INFO");
		ImGui::Text(u8"TILE MAP ATLAS");
		if (nullptr != m_pTexture2D)
		{
			ImGui::Text(u8"Name  : %s", ToString(m_pTexture2D->GetKey()).c_str());
			ImGui::Text(u8"WIDTH : %.3f", m_pTexture2D->Width());
			ImGui::Text(u8"HEIGHT: %.3f", m_pTexture2D->Height());
			if (ImGui::Button("OPEN TILE PALLET")) { m_openTilePallet = true; }
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(" : NONE");
		}
		//End Group Draw atlas Info
		ImGui::EndGroup();
	}

	if (m_openTilePallet)
	{
		DrawPallet();
	}

	//TileInfo
	ImGui::BeginGroup();
	{
		ImGui::Text("TILE SIZE");
		ImGui::SameLine();
		int size[2] = {static_cast<int>(m_vSlicePixel.x), static_cast<int>(m_vSlicePixel.y)};
		if (ImGui::InputInt2("##TILE_SIZE", size))
		{
			size[0] = size[0] <= 0 ? 0 : size[0];
			size[1] = size[1] <= 0 ? 0 : size[1];

			m_vSlicePixel.x = static_cast<float>(size[0]);
			m_vSlicePixel.y = static_cast<float>(size[1]);
		}

		ImGui::Text("X COUNT");
		ImGui::SameLine();
		if (ImGui::InputInt("##TILE_X_COUNT", &m_tileCountX))
		{
			m_tileCountX = m_tileCountX <= 0 ? 1 : m_tileCountX;

			for (size_t i = 0; i < m_tileCountY; ++i)
			{
				m_vecGridCurrentTileID[i].resize(m_tileCountX);
				m_vecTileData[i].resize(m_tileCountX);
			}
		}

		ImGui::Text("Y COUNT");
		ImGui::SameLine();
		if (ImGui::InputInt("##TILE_Y_COUNT", &m_tileCountY))
		{
			m_tileCountY = m_tileCountY <= 0 ? 1 : m_tileCountY;

			const size_t index = m_vecGridCurrentTileID.size();
			m_vecGridCurrentTileID.resize(m_tileCountY);
			m_vecTileData.resize(m_tileCountY);

			if (m_prevCountY < m_tileCountY)
			{
				std::transform(m_vecGridCurrentTileID.begin() + index
				             , m_vecGridCurrentTileID.end()
				             , m_vecGridCurrentTileID.begin() + index
				             , [this](std::vector<int>& row)
				               {
					               row.resize(m_tileCountX);
					               return row;
				               });

				std::transform(m_vecTileData.begin() + index
				             , m_vecTileData.end()
				             , m_vecTileData.begin() + index
				             , [this](std::vector<tTileData>& row)
				               {
					               row.resize(m_tileCountX);
					               return row;
				               });
			}
			m_prevCountY = m_tileCountY;
		}
	}
	ImGui::EndGroup();

	ImGui::Text("SELECTED TILE INDEX : %d", m_selectedTileIndex);
	if (ImGui::Button("RESET")) { Reset(); }

	if (ImGui::Checkbox("LEFT OPEN", &m_openLeft)) {}
	ImGui::SameLine();
	if (ImGui::Checkbox("RIGHT OPEN", &m_openRight)) {}
	if (ImGui::Checkbox("TOP OPEN", &m_openTop)) {}
	ImGui::SameLine();
	if (ImGui::Checkbox("BOTTOM OPEN", &m_openBottom)) {}
	if (ImGui::Button("TEST")) { Test(); }

	if (m_isFailed)
	{
		if (false == m_hasLeft && m_openLeft) { ImGui::Text("NO MAP DATA FOR LEFT"); }
		if (false == m_hasRight && m_openRight) { ImGui::Text("NO MAP DATA FOR RIGHT"); }
		if (false == m_hasTop && m_openTop) { ImGui::Text("NO MAP DATA FOR TOP"); }
		if (false == m_hasBottom && m_openBottom) { ImGui::Text("NO MAP DATA FOR BOTTOM"); }
	}

	if (nullptr != m_pTarget)
	{
		if (ImGui::Button("APPLY TO GAME OBJECT"))
		{
			ApplyToTarget();
		}
	}
	ImGui::EndChild();
}

void TileMapToolUI::DrawCanvasAndGrid()
{
	// 1. 캔버스 그리기
	// 캔버스에 그리드를 표시할지 체크
	const float padding = ImGui::GetStyle().FramePadding.x * 2;

	// 캔버스 위치, 크기 잡기
	const ImVec2 canvasLeftTop = ImGui::GetCursorScreenPos();
	const ImVec2 canvasScale   = ImVec2{
		                             static_cast<float>(m_tileCountX)
		                           , static_cast<float>(m_tileCountY)
	                             } * m_vSlicePixel * m_zoomScale;
	const ImVec2 canvasRightBottom = canvasLeftTop + canvasScale;

	// 캔버스 Outline을 그리고 배경색을 채운다. 
	const ImGuiIO& io        = ImGui::GetIO();
	ImDrawList*    pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(canvasLeftTop, canvasRightBottom, IM_COL32(50, 50, 50, 255));
	pDrawList->AddRect(canvasLeftTop, canvasRightBottom, IM_COL32(255, 255, 255, 255));

	const ImVec2 zoomedTileSize = m_vSlicePixel * m_zoomScale;

	// 그리드 그리기
	pDrawList->PushClipRect(canvasLeftTop, canvasRightBottom, true);
	// 그리드 크기
	const ImVec2 GRID_STEP = zoomedTileSize;
	// 가로선 그리기
	for (float xStep = fmodf(0, GRID_STEP.x); xStep < canvasScale.x; xStep += GRID_STEP.x)
	{
		pDrawList->AddLine(ImVec2(canvasLeftTop.x + xStep, canvasLeftTop.y)
		                 , ImVec2(canvasLeftTop.x + xStep, canvasRightBottom.y)
		                 , IM_COL32(200, 200, 200, 40));
	}
	// 세로선 그리기
	for (float yStep = fmodf(0, GRID_STEP.y); yStep < canvasScale.y; yStep += GRID_STEP.y)
	{
		pDrawList->AddLine(ImVec2(canvasLeftTop.x, canvasLeftTop.y + yStep)
		                 , ImVec2(canvasRightBottom.x, canvasLeftTop.y + yStep)
		                 , IM_COL32(200, 200, 200, 40));
	}

	if (nullptr != m_pTexture2D)
	{
		m_vSliceUV = m_vSlicePixel / m_pTexture2D->GetSize();
		m_colCount = static_cast<int>(m_pTexture2D->Width() / m_vSlicePixel.x);
		m_rowCount = static_cast<int>(m_pTexture2D->Height() / m_vSlicePixel.y);

		// 타일맵 그리기
		for (int row = 0; row < m_tileCountY; row++)
		{
			for (int col = 0; col < m_tileCountX; col++)
			{
				// 그릴 위치 찾기
				ImVec2 leftTop     = canvasLeftTop + ImVec2{zoomedTileSize.x * col, zoomedTileSize.y * row};
				ImVec2 rightBottom = leftTop + zoomedTileSize;

				// 타일 데이터 가져오기
				tTileData currentTile = m_vecTileData[row][col];

				// 타일의 이미지 인덱스가 -1이면 그리지 않는다.
				if (currentTile.iImgIdx != -1)
				{
					// Right-Bottom UV 계산하기
					ImVec2 rightBottomUV = currentTile.vLTUV + m_vSliceUV;

					// 타일 그리기
					pDrawList->AddImage(m_pTexture2D->GetSRV().Get()
					                  , leftTop
					                  , rightBottom
					                  , currentTile.vLTUV
					                  , rightBottomUV);
				}
			}
		}

		// 캔버스가 사용자의 마우스 입력을 감지하도록 버튼 추가
		ImGui::InvisibleButton("canvas"
		                     , canvasScale
		                     , ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
		                       ImGuiButtonFlags_MouseButtonMiddle);

		// 마우스 상태 저장 변수들
		const bool   is_hovered       = ImGui::IsItemHovered(); // Hovered
		const bool   is_active        = ImGui::IsItemActive();   // Held
		const ImVec2 mousePosInCanvas = io.MousePos - canvasLeftTop;

		// 마우스 동작
		if (false == is_hovered) return;

		// 마우스 위치가 canvas안 이라면
		if (0 < mousePosInCanvas.x && mousePosInCanvas.x < canvasScale.x
		    && 0 < mousePosInCanvas.y && mousePosInCanvas.y < canvasScale.y)
		{
			// 마우스 위치에 있는 타일 인덱스 구하기
			const int col = static_cast<int>(mousePosInCanvas.x / zoomedTileSize.x);
			const int row = static_cast<int>(mousePosInCanvas.y / zoomedTileSize.y);

			// 마우스 위치에 있는 타일 하이라이트
			const ImVec2 leftTop     = canvasLeftTop + ImVec2{zoomedTileSize.x * col, zoomedTileSize.y * row};
			const ImVec2 rightBottom = leftTop + zoomedTileSize;
			pDrawList->AddRectFilled(leftTop, rightBottom, IM_COL32(120, 120, 120, 120));

			// 좌클릭 => 타일 그리기
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				if (INVALID_TILE_INDEX != m_vecTileData[row][col].iImgIdx && KEY_PRESSED(KEY::LCTRL))
				{
					m_imageIndex        = m_vecTileData[row][col].iImgIdx;
					m_selectedTileIndex = m_imageIndex;
				}
				m_vecTileData[row][col].iImgIdx = m_selectedTileIndex;

				if (m_selectedTileIndex != INVALID_TILE_INDEX)
				{
					const int  selectedCol = m_selectedTileIndex % m_colCount;
					const int  selectedRow = m_selectedTileIndex / m_colCount;
					const Vec2 leftTopUV   = Vec2{selectedCol, selectedRow} * Vec2{m_vSliceUV.x, m_vSliceUV.y};

					m_vecTileData[row][col].vLTUV = leftTopUV;
				}
			}

			// 우클릭 => 타일 지우기, 
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
			         ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				m_vecTileData[row][col].iImgIdx = INVALID_TILE_INDEX;
			}
		}
		// 마우스 휠 => 크기 조정
		if (KEY_PRESSED(KEY::LCTRL))
		{
			// Minimum zoom = x0.1 (near camera limit)
			// Maximum zoom = x5.0 (it can fit an image with 1600px width)
			m_zoomScale = m_zoomScale + (io.MouseWheel * 0.3f);
			m_zoomScale = std::clamp(m_zoomScale, 0.1f, 5.0f);
		}
	}


	pDrawList->PopClipRect();
}

void TileMapToolUI::DrawPallet()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
	ImGui::Begin("TILE_PALLET", &m_openTilePallet, ImGuiWindowFlags_HorizontalScrollbar);
	DrawImage();
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void TileMapToolUI::DrawImage()
{
	if (nullptr != m_pTexture2D)
	{
		m_colCount                 = static_cast<int>(m_pTexture2D->Width() / m_vSlicePixel.x);
		m_rowCount                 = static_cast<int>(m_pTexture2D->Height() / m_vSlicePixel.y);
		const ImVec2 canvasLeftTop = ImGui::GetCursorScreenPos();
		const ImVec2 canvasScale   = ImVec2{
			                             static_cast<float>(m_colCount)
			                           , static_cast<float>(m_rowCount)
		                             } * m_vSlicePixel * m_zoomScale;
		const ImVec2 canvasRightBottom = canvasLeftTop + canvasScale;

		ImGui::Image(m_pTexture2D->GetSRV().Get(), m_pTexture2D->GetSize() * m_zoomScale);

		// 그리드 크기


		const ImVec2 zoomedTileSize = m_vSlicePixel * m_zoomScale;
		const ImVec2 GRID_STEP      = zoomedTileSize;
		ImDrawList*  pDrawList      = ImGui::GetWindowDrawList();
		// 가로선 그리기
		for (float xStep = fmodf(0, GRID_STEP.x); xStep < canvasScale.x; xStep += GRID_STEP.x)
		{
			pDrawList->AddLine(ImVec2(canvasLeftTop.x + xStep, canvasLeftTop.y)
			                 , ImVec2(canvasLeftTop.x + xStep, canvasRightBottom.y)
			                 , IM_COL32(255, 0, 0, 255));
		}
		// 세로선 그리기
		for (float yStep = fmodf(0, GRID_STEP.y); yStep < canvasScale.y; yStep += GRID_STEP.y)
		{
			pDrawList->AddLine(ImVec2(canvasLeftTop.x, canvasLeftTop.y + yStep)
			                 , ImVec2(canvasRightBottom.x, canvasLeftTop.y + yStep)
			                 , IM_COL32(255, 0, 0, 255));
		}
		// 마우스 상태 저장 변수들
		const ImGuiIO& io               = ImGui::GetIO();
		const bool     is_hovered       = ImGui::IsItemHovered(); // Hovered
		const ImVec2   mousePosInCanvas = io.MousePos - canvasLeftTop;

		// 캔버스가 사용자의 마우스 입력을 감지하도록 버튼 추가
		ImGui::InvisibleButton("canvas"
		                     , canvasScale
		                     , ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
		                       ImGuiButtonFlags_MouseButtonMiddle);

		if (false == is_hovered) return;

		// 마우스 위치가 canvas안 이라면
		if (0 < mousePosInCanvas.x && mousePosInCanvas.x < canvasScale.x
		    && 0 < mousePosInCanvas.y && mousePosInCanvas.y < canvasScale.y)
		{
			// 마우스 위치에 있는 타일 인덱스 구하기
			const int col = static_cast<int>(mousePosInCanvas.x / zoomedTileSize.x);
			const int row = static_cast<int>(mousePosInCanvas.y / zoomedTileSize.y);

			// 마우스 위치에 있는 타일 하이라이트
			const ImVec2 leftTop     = canvasLeftTop + ImVec2{zoomedTileSize.x * col, zoomedTileSize.y * row};
			const ImVec2 rightBottom = leftTop + zoomedTileSize;
			pDrawList->AddRectFilled(leftTop, rightBottom, IM_COL32(120, 120, 120, 120));

			// 좌클릭 => 타일 그리기
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				m_selectedTileIndex = col + row * m_colCount;
			}
		}

		// 마우스 휠 => 크기 조정
		if (KEY_PRESSED(KEY::LCTRL))
		{
			// Minimum zoom = x0.1 (near camera limit)
			// Maximum zoom = x5.0 (it can fit an image with 1600px width)
			m_zoomScale = m_zoomScale + (io.MouseWheel * 0.3f);
			m_zoomScale = std::clamp(m_zoomScale, 0.1f, 5.0f);
		}
	}
}

void TileMapToolUI::RoomMetaFromXML()
{
	OpenFileDialog(XML_FILTER_FORMAT, 0, L"meta");
	if (m_filePath.empty()) return;

	CMarkup xml{};
	bool    result = xml.Load(m_filePath);
	if (false == result) { assert(nullptr); }

	xml.FindElem(L"tilemap");

	//width
	std::wstring width  = xml.GetAttrib(L"width");
	m_tileCountX        = std::stoi(width);
	std::wstring height = xml.GetAttrib(L"height");
	m_tileCountY        = std::stoi(height);

	m_vecGridCurrentTileID.clear();

	m_vecGridCurrentTileID.resize(m_tileCountY);
	for (int i = 0; i < m_vecGridCurrentTileID.size(); ++i)
	{
		m_vecGridCurrentTileID[i].resize(m_tileCountX);
	}
	m_vecCleanTile = m_vecGridCurrentTileID;

	m_mapGridTiles[L"main"]         = m_vecGridCurrentTileID;
	m_mapGridTiles[L"left_open"]    = m_vecGridCurrentTileID;
	m_mapGridTiles[L"left_close"]   = m_vecGridCurrentTileID;
	m_mapGridTiles[L"right_open"]   = m_vecGridCurrentTileID;
	m_mapGridTiles[L"right_close"]  = m_vecGridCurrentTileID;
	m_mapGridTiles[L"top_open"]     = m_vecGridCurrentTileID;
	m_mapGridTiles[L"top_close"]    = m_vecGridCurrentTileID;
	m_mapGridTiles[L"bottom_open"]  = m_vecGridCurrentTileID;
	m_mapGridTiles[L"bottom_close"] = m_vecGridCurrentTileID;

	m_mapGridWalls[L"main"]         = m_vecGridCurrentTileID;
	m_mapGridWalls[L"left_open"]    = m_vecGridCurrentTileID;
	m_mapGridWalls[L"left_close"]   = m_vecGridCurrentTileID;
	m_mapGridWalls[L"right_open"]   = m_vecGridCurrentTileID;
	m_mapGridWalls[L"right_close"]  = m_vecGridCurrentTileID;
	m_mapGridWalls[L"top_open"]     = m_vecGridCurrentTileID;
	m_mapGridWalls[L"top_close"]    = m_vecGridCurrentTileID;
	m_mapGridWalls[L"bottom_open"]  = m_vecGridCurrentTileID;
	m_mapGridWalls[L"bottom_close"] = m_vecGridCurrentTileID;

	xml.IntoElem();
	while (xml.FindElem(L"layer"))
	{
		if (L"tile" == xml.GetAttrib(L"type"))
		{
			std::wstring dir      = xml.GetAttrib(L"dir");
			std::wstring openType = xml.GetAttrib(L"opentype");

			std::vector<std::vector<int>>* pGrid{};

			if (L"main" == dir)
				pGrid = &m_mapGridTiles[L"main"];
			else if (L"left" == dir)
			{
				m_hasLeft = true;
				pGrid     = (L"closed" == openType) ? &m_mapGridTiles[L"left_close"] : &m_mapGridTiles[L"left_open"];
			}
			else if (L"right" == dir)
			{
				m_hasRight = true;
				pGrid      = (L"closed" == openType) ? &m_mapGridTiles[L"right_close"] : &m_mapGridTiles[L"right_open"];
			}
			else if (L"top" == dir)
			{
				m_hasTop = true;
				pGrid    = (L"closed" == openType) ? &m_mapGridTiles[L"top_close"] : &m_mapGridTiles[L"top_open"];
			}
			else if (L"bottom" == dir)
			{
				m_hasBottom = true;
				pGrid = (L"closed" == openType) ? &m_mapGridTiles[L"bottom_close"] : &m_mapGridTiles[L"bottom_open"];
			}

			std::wstringstream ss{xml.GetElemContent()};
			std::wstring       line{};

			int row = 0;
			while (std::getline(ss, line, L'\r'))
			{
				std::wstringstream lineSS{line};
				std::wstring       tileID{};
				int                col = 0;
				while (lineSS >> tileID)
				{
					(*pGrid)[row][col] = std::stoi(tileID);
					++col;
				}
				++row;
			}
		}
		else if (L"wall" == xml.GetAttrib(L"type"))
		{
			std::wstring dir      = xml.GetAttrib(L"dir");
			std::wstring openType = xml.GetAttrib(L"opentype");

			std::vector<std::vector<int>>* pGrid{};

			if (L"main" == dir)
				pGrid = &m_mapGridWalls[L"main"];
			else if (L"left" == dir)
				pGrid = (L"closed" == openType) ? &m_mapGridWalls[L"left_close"] : &m_mapGridWalls[L"left_open"];
			else if (L"right" == dir)
				pGrid = (L"closed" == openType) ? &m_mapGridWalls[L"right_close"] : &m_mapGridWalls[L"right_open"];
			else if (L"top" == dir)
				pGrid = (L"closed" == openType) ? &m_mapGridWalls[L"top_close"] : &m_mapGridWalls[L"top_open"];
			else if (L"bottom" == dir)
				pGrid = (L"closed" == openType) ? &m_mapGridWalls[L"bottom_close"] : &m_mapGridWalls[L"bottom_open"];

			std::wstringstream ss{xml.GetElemContent()};
			std::wstring       line{};

			int row = 0;
			while (std::getline(ss, line, L'\r'))
			{
				std::wstringstream lineSS{line};
				std::wstring       tileID{};
				int                col = 0;
				while (lineSS >> tileID)
				{
					(*pGrid)[row][col] = std::stoi(tileID);
					++col;
				}
				++row;
			}
		}
	}
	m_vecTileData.resize(m_tileCountY);
	for (int i = 0; i < m_vecTileData.size(); ++i)
	{
		m_vecTileData[i].resize(m_tileCountX);
	}
}

void TileMapToolUI::LoadTileMetaInfo()
{
	OpenFileDialog(TXT_FILTER_FORMAT, 0, L"meta");
	if (m_filePath.empty()) return;

	std::wifstream inputFile{m_filePath};
	if (false == inputFile.is_open()) { assert(nullptr); }

	while (false == inputFile.eof())
	{
		DunGreedTileInfo tileInfo{};
		inputFile >> tileInfo.name;
		inputFile >> tileInfo.position.x >> tileInfo.position.y;
		inputFile >> tileInfo.scale.x >> tileInfo.scale.y;
		inputFile >> tileInfo.offset.x >> tileInfo.offset.y;
		inputFile >> tileInfo.tileID;
		inputFile >> tileInfo.oneWay;
		inputFile >> tileInfo.tileType;

		m_mapDunGreedTileInfo.insert({tileInfo.tileID, std::move(tileInfo)});
	}
}

//
//void TileMapToolUI::LoadMap()
//{
//	/*m_vecGridCurrentTileID.clear();
//
//	m_pTexture2D               = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\townAtlas1.png");
//	std::wstring   contentPath = CPathMgr::GetInst()->GetContentPath();
//	std::wstring   fileName    = L"meta/tile/Tile.txt";
//	std::wifstream inputFile{contentPath + fileName};
//	if (false == inputFile.is_open()) { assert(nullptr); }
//
//	while (false == inputFile.eof())
//	{
//		DunGreedTileInfo tileInfo{};
//		inputFile >> tileInfo.name;
//		inputFile >> tileInfo.position.x >> tileInfo.position.y;
//		inputFile >> tileInfo.scale.x >> tileInfo.scale.y;
//		inputFile >> tileInfo.offset.x >> tileInfo.offset.y;
//		inputFile >> tileInfo.tileID;
//		inputFile >> tileInfo.oneWay;
//		inputFile >> tileInfo.tileType;
//
//		m_mapDunGreedTileInfo.insert({tileInfo.tileID, std::move(tileInfo)});
//	}
//	inputFile.close();
//
//	OpenFileDialog(TXT_FILTER_FORMAT, 0, L"meta");
//	if (m_filePath.empty()) return;
//	inputFile.open(m_filePath);
//	inputFile >> m_tileCountX;
//	inputFile >> m_tileCountY;
//	m_prevCountY = m_tileCountY;
//	
//	m_vecGridCurrentTileID.resize(m_tileCountY);
//	for (int i = 0; i < m_tileCountY; ++i)
//	{
//		m_vecGridCurrentTileID[i].resize(m_tileCountX);
//	}
//	for (int i = 0; i < m_tileCountY; ++i)
//	{
//		for (int j = 0; j < m_tileCountX; ++j)
//		{
//			inputFile >> m_vecGridCurrentTileID[i][j];
//		}
//	}
//	m_vecTileData.resize(m_tileCountY);
//	for (int i = 0; i < m_tileCountY; ++i)
//	{
//		m_vecTileData[i].resize(m_tileCountX);
//	}
//
//	for (int i = 0; i < m_tileCountY; ++i)
//	{
//		for (int j = 0; j < m_tileCountX; ++j)
//		{
//			auto iter = m_mapDunGreedTileInfo.find(m_vecGridCurrentTileID[i][j]);
//			if (iter == m_mapDunGreedTileInfo.end()) continue;
//
//			DunGreedTileInfo& tileInfo = iter->second;
//			m_vecTileData[i][j].vLTUV  = tileInfo.position / m_pTexture2D->GetSize();
//
//			const int col               = static_cast<int>(tileInfo.position.x) / static_cast<int>(m_vSlicePixel.x);
//			const int row               = static_cast<int>(tileInfo.position.y) / static_cast<int>(m_vSlicePixel.y);
//			const int imageIndex        = col + row * m_colCount;
//			m_vecTileData[i][j].iImgIdx = imageIndex;
//		}
//	}*/
//}

void TileMapToolUI::New()
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
		LoadTexture();
	}
}

void TileMapToolUI::Reset()
{
	for (int i = 0; i < m_tileCountY; ++i)
	{
		for (int j = 0; j < m_tileCountX; ++j)
		{
			m_vecTileData[i][j] = {};
		}
	}
	m_isFailed = false;
}

void TileMapToolUI::Save()
{
	OpenFileDialog(TILE_FILTER_FORMAT, 0, L"tile");
	if (m_filePath.empty()) return;

	std::ofstream outputFile{m_filePath, std::ios_base::binary};
	if (outputFile.is_open())
	{
		//TileMap 이름 저장
		const std::wstring fileName       = std::filesystem::path{m_filePath}.filename();
		int                fileNameLength = static_cast<int>(fileName.size());
		outputFile.write((const char*)&fileNameLength, sizeof(int));
		//애니메이션 사용자(컨트롤러)이름 저장
		outputFile.write((const char*)fileName.c_str(), sizeof(wchar_t) * fileNameLength);
		//vSlicePixel 저장
		outputFile.write((const char*)&m_vSlicePixel, sizeof(Vec2));
		//vSliceUV 저장
		outputFile.write((const char*)&m_vSliceUV, sizeof(Vec2));
		//rowCount 저장
		outputFile.write((const char*)&m_rowCount, sizeof(int));
		//colCount 저장
		outputFile.write((const char*)&m_colCount, sizeof(int));
		//tileCountX 저장
		outputFile.write((const char*)&m_tileCountX, sizeof(int));
		//tileCountY 저장
		outputFile.write((const char*)&m_tileCountY, sizeof(int));
		//tileData 저장
		std::vector<tTileData> vecData{};
		vecData.reserve(m_tileCountX * m_tileCountY);
		for (int i = 0; i < m_tileCountY; ++i)
		{
			for (int j = 0; j < m_tileCountX; ++j)
			{
				vecData.push_back(m_vecTileData[i][j]);
			}
		}
		outputFile.write((const char*)vecData.data(), sizeof(tTileData) * vecData.size());
	}
	else
	{
		assert(nullptr);
	}
}

void TileMapToolUI::LoadTileFile()
{
	OpenFileDialog(TILE_FILTER_FORMAT, 0, L"tile");
	if (m_filePath.empty()) return;

	std::ifstream inputFile{m_filePath, std::ios_base::binary};
	if (inputFile.is_open())
	{
		//TileMap 이름 저장
		int fileNameLength{};
		inputFile.read((char*)&fileNameLength, sizeof(int));
		//타일맵 사용자(컨트롤러)이름
		std::wstring fileName{};
		fileName.resize(fileNameLength);
		inputFile.read((char*)fileName.data(), sizeof(wchar_t) * fileNameLength);
		//vSlicePixel
		inputFile.read((char*)&m_vSlicePixel, sizeof(Vec2));
		//vSliceUV
		inputFile.read((char*)&m_vSliceUV, sizeof(Vec2));
		//rowCount
		inputFile.read((char*)&m_rowCount, sizeof(int));
		//colCount
		inputFile.read((char*)&m_colCount, sizeof(int));
		//tileCountX
		inputFile.read((char*)&m_tileCountX, sizeof(int));
		//tileCountY
		inputFile.read((char*)&m_tileCountY, sizeof(int));
		//tileData 
		std::vector<tTileData> vecData{};
		vecData.resize(m_tileCountX * m_tileCountY);
		inputFile.read((char*)vecData.data(), sizeof(tTileData) * vecData.size());

		m_vecTileData.resize(m_tileCountY);
		for (int i = 0; i < m_tileCountY; ++i)
		{
			m_vecTileData[i].resize(m_tileCountX);
		}

		for (int i = 0; i < m_tileCountY; ++i)
		{
			for (int j = 0; j < m_tileCountX; ++j)
			{
				m_vecTileData[i][j] = vecData[j + i * m_tileCountX];
			}
		}
	}
	else
	{
		assert(nullptr);
	}
}

void TileMapToolUI::Test()
{
	if (0 == m_mapDunGreedTileInfo.size()) { return; }
	//1. main은 항상 적용
	m_vecGridCurrentTileID = m_vecCleanTile;
	ApplyTile(L"main");
	ApplyWall(L"main");
	Apply();

	for (int i = 0; i < m_tileCountY; ++i)
	{
		for (int j = 0; j < m_tileCountX; ++j)
		{
			auto iter = m_mapDunGreedTileInfo.find(m_vecGridCurrentTileID[i][j]);
			assert(iter != m_mapDunGreedTileInfo.end());

			DunGreedTileInfo& tileInfo = iter->second;
			m_vecTileData[i][j].vLTUV  = tileInfo.position / m_pTexture2D->GetSize();

			const int col               = static_cast<int>(tileInfo.position.x) / static_cast<int>(m_vSlicePixel.x);
			const int row               = static_cast<int>(tileInfo.position.y) / static_cast<int>(m_vSlicePixel.y);
			const int imageIndex        = col + row * m_colCount;
			m_vecTileData[i][j].iImgIdx = imageIndex;
		}
	}
}

void TileMapToolUI::Apply()
{
	if ((false == m_hasLeft && true == m_openLeft)
	    || (false == m_hasRight && true == m_openRight)
	    || (false == m_hasTop && true == m_openTop)
	    || (false == m_hasBottom && true == m_openBottom))
	{
		m_isFailed = true;
		return;
	}

	if (m_hasLeft)
	{
		if (m_openLeft)
		{
			ApplyTile(L"left_open");
			ApplyWall(L"left_open");
		}
		else
		{
			ApplyTile(L"left_close");
			ApplyWall(L"left_close");
		}
	}
	if (m_hasRight)
	{
		if (m_openRight)
		{
			ApplyTile(L"right_open");
			ApplyWall(L"right_open");
		}
		else
		{
			ApplyTile(L"right_close");
			ApplyWall(L"right_close");
		}
	}
	if (m_hasTop)
	{
		if (m_openTop)
		{
			ApplyTile(L"top_open");
			ApplyWall(L"top_open");
		}
		else
		{
			ApplyTile(L"top_close");
			ApplyWall(L"top_close");
		}
	}
	if (m_hasBottom)
	{
		if (m_openBottom)
		{
			ApplyTile(L"bottom_open");
			ApplyWall(L"bottom_open");
		}
		else
		{
			ApplyTile(L"bottom_close");
			ApplyWall(L"bottom_close");
		}
	}
}

void TileMapToolUI::ApplyTile(const std::wstring& key)
{
	if (0 == m_mapGridTiles.size()) { return; }

	for (int i = 0; i < m_tileCountY; ++i)
	{
		for (int j = 0; j < m_tileCountX; ++j)
		{
			if (0 != m_mapGridTiles[key][i][j])
			{
				m_vecGridCurrentTileID[i][j] = m_mapGridTiles[key][i][j];
			}
		}
	}
}

void TileMapToolUI::ApplyWall(const std::wstring& key)
{
	if (0 == m_mapGridWalls.size()) { return; }

	for (int i = 0; i < m_tileCountY; ++i)
	{
		for (int j = 0; j < m_tileCountX; ++j)
		{
			if (0 != m_mapGridWalls[key][i][j])
			{
				m_vecGridCurrentTileID[i][j] = m_mapGridWalls[key][i][j];
			}
		}
	}
}

void TileMapToolUI::ApplyToTarget()
{
	if (nullptr != m_pTarget)
	{
		CTileMap* pTileMap = m_pTarget->TileMap();
		pTileMap->SetAtlasTex(m_pTexture2D);
		pTileMap->SetTileMapCount(m_tileCountX, m_tileCountY);
		pTileMap->SetTileSize(Vec2{m_vSlicePixel.x, m_vSlicePixel.y});
		std::vector<tTileData> vecData{};
		vecData.reserve(m_tileCountX * m_tileCountY);
		for (int i = 0; i < m_tileCountY; ++i)
		{
			for (int j = 0; j < m_tileCountX; ++j)
			{
				vecData.push_back(m_vecTileData[i][j]);
			}
		}
		pTileMap->SetTileData(vecData);
	}

	Deactivate();
}
