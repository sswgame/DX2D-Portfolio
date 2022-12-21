#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine/CDevice.h>
#include <Engine/CGameObject.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "UI.h"
#include "ParamUI.h"

CImGuiMgr::CImGuiMgr()
	:
	m_hNotify{nullptr} {}

CImGuiMgr::~CImGuiMgr()
{
	Safe_Del_Map(m_mapUI);
}

void CImGuiMgr::init(HWND hwnd)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(DEVICE, CONTEXT);

	// 기본 UI 들 생성
	CreateUI();

	// 알림설정  
	const std::wstring path = CPathMgr::GetInst()->GetContentPath();
	m_hNotify               = FindFirstChangeNotification(path.c_str()
	                                                    , TRUE
	                                                    , FILE_NOTIFY_CHANGE_FILE_NAME
	                                                      | FILE_NOTIFY_CHANGE_DIR_NAME
	                                                      | FILE_ACTION_ADDED
	                                                      | FILE_ACTION_REMOVED);
}

void CImGuiMgr::progress()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Param Init
	ParamUI::KeyCount = 0;

	// UI Update
	for (const auto& [UIName, pUI] : m_mapUI)
	{
		pUI->update();
	}
	// UI Render
	for (const auto& [UIName, pUI] : m_mapUI)
	{
		pUI->render();
	}

	//bool bOpen = true;
	//ImGui::ShowDemoWindow(&bOpen);

	// Delegate 호출
	for (auto& [pFunc, pParameter] : m_vecDelegate)
	{
		pFunc(pParameter);
	}

	m_vecDelegate.clear();

	// Content 변경 감지
	ObserveContent();
}


void CImGuiMgr::render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	const ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}


void CImGuiMgr::clear()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#include "MenuUI.h"
#include "InspectorUI.h"
#include "ResourceUI.h"
#include "SceneOutLiner.h"
#include "ListUI.h"
#include "TileMapToolUI.h"
#include "AnimationToolUI.h"
#include "TextureToolUI.h"

void CImGuiMgr::CreateUI()
{
	// MenuBar 생성
	MenuUI* pMenuUI = new MenuUI{};
	pMenuUI->Activate();
	m_mapUI.insert(make_pair(pMenuUI->GetName(), pMenuUI));

	// InspectorUI 생성
	InspectorUI* pUI = new InspectorUI{};
	pUI->SetTargetObject(nullptr);
	m_mapUI.insert(make_pair(pUI->GetName(), pUI));

	// ResourceUI 생성
	ResourceUI* pResUI = new ResourceUI{};
	m_mapUI.insert(make_pair(pResUI->GetName(), pResUI));

	// SceneOutLiner
	SceneOutLiner* pSceneOutLiner = new SceneOutLiner{};
	m_mapUI.insert(make_pair(pSceneOutLiner->GetName(), pSceneOutLiner));

	// ListUI 생성
	ListUI* pListUI = new ListUI{};
	pListUI->Deactivate();
	pListUI->SetModalUI(true);
	m_mapUI.insert(make_pair(pListUI->GetName(), pListUI));

	//AnimationToolUI
	AnimationToolUI* pAnimationToolUI = new AnimationToolUI{};
	pAnimationToolUI->Deactivate();
	pAnimationToolUI->SetImGuiWindowFlags(ImGuiWindowFlags_MenuBar
	                                      | ImGuiWindowFlags_NoScrollWithMouse);
	m_mapUI.insert(std::make_pair(pAnimationToolUI->GetName(), pAnimationToolUI));

	//TileMapToolUI
	TileMapToolUI* pTileMapToolUI = new TileMapToolUI{};
	pTileMapToolUI->Deactivate();
	pTileMapToolUI->SetImGuiWindowFlags(ImGuiWindowFlags_MenuBar);
	m_mapUI.insert(std::make_pair(pTileMapToolUI->GetName(), pTileMapToolUI));

	//AnimationToolUI
	TextureToolUI* pTextureToolUI = new TextureToolUI{};
	pTextureToolUI->Deactivate();
	pTextureToolUI->SetImGuiWindowFlags(ImGuiWindowFlags_MenuBar
	                                    | ImGuiWindowFlags_NoScrollWithMouse);
	m_mapUI.insert(std::make_pair(pTextureToolUI->GetName(), pTextureToolUI));
}


UI* CImGuiMgr::FindUI(const string& key)
{
	const auto iter = m_mapUI.find(key);
	return (m_mapUI.end() == iter) ? nullptr : iter->second;
}


void CImGuiMgr::ObserveContent()
{
	const DWORD waitStatus = WaitForSingleObject(m_hNotify, 0);

	if (WAIT_OBJECT_0 == waitStatus)
	{
		ResourceUI* pResUI = static_cast<ResourceUI*>(FindUI("Resource"));
		pResUI->Reset();

		const std::wstring path = CPathMgr::GetInst()->GetContentPath();
		FindNextChangeNotification(m_hNotify);
	}
}
