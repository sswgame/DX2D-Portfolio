#include "pch.h"
#include "CTestScene.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Script/CSceneSaveLoad.h>

void CTestScene::CreateTestScene()
{
	const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	CScene*            pLoadScene  = CSceneSaveLoad::LoadScene(contentPath + L"scene\\0.Title.scene");
	CSceneMgr::GetInst()->GoToNextScene(pLoadScene, false);
}
