#include "pch.h"
#include "MenuUI.h"

#include <Engine/CCore.h>
#include <Engine/CPathMgr.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>

#include <Script/CSceneSaveLoad.h>

#include "AnimationToolUI.h"
#include "CImGuiMgr.h"
#include "SceneOutLiner.h"
#include "TileMapToolUI.h"

MenuUI::MenuUI()
	:
	UI{"Menu"}
  , m_bPackaging(false) {}

MenuUI::~MenuUI() = default;


void MenuUI::render()
{
	if (ImGui::BeginMainMenuBar())
	{
		render_update();

		ImGui::EndMainMenuBar();
	}
}

void MenuUI::render_update()
{
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("Packaging", nullptr, &m_bPackaging);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scene"))
	{
		if (ImGui::MenuItem("Scene Save")) { Save(); }
		if (ImGui::MenuItem("Scene Load")) { Load(); }

		const CScene*     pCurrentScene = CSceneMgr::GetInst()->GetCurScene();
		const SCENE_STATE sceneState    = pCurrentScene->GetSceneState();

		if (SCENE_STATE::PLAY == sceneState)
		{
			if (ImGui::MenuItem("Pause")) { Pause(); }
		}
		else
		{
			if (ImGui::MenuItem("Play")) { Play(); }
		}

		if (ImGui::MenuItem("Stop"
		                  , nullptr
		                  , false
		                  , (SCENE_STATE::STOP != sceneState)))
		{
			Stop();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("AnimationTool"))
		{
			AnimationToolUI* pAnimationToolUI
				= static_cast<AnimationToolUI*>(CImGuiMgr::GetInst()->FindUI("AnimationTool"));
			assert(pAnimationToolUI);
			if (false == pAnimationToolUI->IsActive())
			{
				pAnimationToolUI->Activate();
				pAnimationToolUI->SetTarget(nullptr);
			}
		}
		if (ImGui::MenuItem("TileMapTool"))
		{
			TileMapToolUI* pTileMapToolUI
				= static_cast<TileMapToolUI*>(CImGuiMgr::GetInst()->FindUI("TileMapTool"));
			assert(pTileMapToolUI);
			if (false == pTileMapToolUI->IsActive())
			{
				pTileMapToolUI->Activate();
				pTileMapToolUI->SetTarget(nullptr);
			}
		}
		ImGui::EndMenu();
	}
}

void MenuUI::Save()
{
	wchar_t fileName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize    = sizeof(OPENFILENAME);
	ofn.hwndOwner      = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile      = fileName;
	ofn.nMaxFile       = sizeof(fileName);
	ofn.lpstrFilter    = L"ALL\0*.*\0Scene\0*.scene\0";
	ofn.nFilterIndex   = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle  = 0;

	std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	contentPath += L"scene";

	ofn.lpstrInitialDir = contentPath.c_str();
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetSaveFileName(&ofn))
	{
		CSceneSaveLoad::SaveScene(CSceneMgr::GetInst()->GetCurScene(), ofn.lpstrFile);
	}
}

void MenuUI::Load()
{
	wchar_t fileName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize    = sizeof(OPENFILENAME);
	ofn.hwndOwner      = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile      = fileName;
	ofn.nMaxFile       = sizeof(fileName);
	ofn.lpstrFilter    = L"ALL\0*.*\0Tile\0*.tile\0";
	ofn.nFilterIndex   = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle  = 0;

	std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	contentPath += L"scene";

	ofn.lpstrInitialDir = contentPath.c_str();
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetOpenFileName(&ofn))
	{
		CScene* pLoadScene = CSceneSaveLoad::LoadScene(ofn.lpstrFile);
		CSceneMgr::GetInst()->GoToNextScene(pLoadScene,false);
	}

	// SceneOutLiner 갱신
	SceneOutLiner* pUI = static_cast<SceneOutLiner*>(CImGuiMgr::GetInst()->FindUI("SceneOutLiner"));
	pUI->Reset();
}

void MenuUI::Play()
{
	CSceneMgr::GetInst()->GetCurScene()->SetSceneState(SCENE_STATE::PLAY);
}

void MenuUI::Pause()
{
	CSceneMgr::GetInst()->GetCurScene()->SetSceneState(SCENE_STATE::PAUSE);
}

void MenuUI::Stop()
{
	CScene*     pCurScene = CSceneMgr::GetInst()->GetCurScene();
	SCENE_STATE eState    = pCurScene->GetSceneState();

	if (SCENE_STATE::STOP != eState)
	{
		pCurScene->SetSceneState(SCENE_STATE::STOP);
		CSceneFile* pSceneFile = pCurScene->GetSceneFile().Get();

		const std::wstring filePath  = CPathMgr::GetInst()->GetContentPath() + pSceneFile->GetRelativePath();
		CScene*            pNewScene = CSceneSaveLoad::LoadScene(filePath);

		CSceneMgr::GetInst()->ClearDonDestroyOnLoad();
		CSceneMgr::GetInst()->GoToNextScene(pNewScene,false);
	}

	// SceneOutLiner 갱신
	static_cast<SceneOutLiner*>(CImGuiMgr::GetInst()->FindUI("SceneOutLiner"))->Reset();
	ShowCursor(SW_SHOW);
}
