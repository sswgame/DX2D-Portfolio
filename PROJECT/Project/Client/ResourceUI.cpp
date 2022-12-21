#include "pch.h"
#include "ResourceUI.h"

#include <Engine/CEventMgr.h>
#include <Engine/CPrefab.h>
#include <Engine/CResMgr.h>
#include <Engine/CScene.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CSound.h>

#include <Script/CSceneSaveLoad.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"
#include "TreeUI.h"

ResourceUI::ResourceUI()
	:
	UI("Resource")
{
	m_TreeUI = new TreeUI(true);
	m_TreeUI->SetTitle("Resource");
	m_TreeUI->UseFrame(true);
	m_TreeUI->UseDragDropOuter(true);
	AddChild(m_TreeUI);

	//m_TreeUI->SetAcceptDragUIName("SceneOutLiner");

	// Clicked Delegate 등록
	m_TreeUI->SetClickedDelegate(BIND_FN(ResourceUI::ItemClicked));
	// DoubleClicked Deletage 등록
	m_TreeUI->SetDoubleClickedDelegate(BIND_FN(ResourceUI::ItemDBClicked));

	Reset();
}

ResourceUI::~ResourceUI() {}

void ResourceUI::update()
{
	// 리소스 변경상태 저장
	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::S))
	{
		CResMgr::GetInst()->SaveChangedRes();
	}

	if (nullptr != m_pSelectedNode && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		ImGui::OpenPopup("RESOURCE_POPUP");
	}
	if (ImGui::BeginPopup("RESOURCE_POPUP"))
	{
		static const std::string menus[] = {"Delete"};
		for (int i = 0; i < std::size(menus); ++i)
		{
			if (ImGui::Selectable(menus[i].c_str()))
			{
				if ("Delete" == menus[i] && m_pSelectedNode)
					DeleteResourceFile(m_pSelectedNode);
			}
		}
		ImGui::EndPopup();
	}
	UI::update();
}

void ResourceUI::render_update()
{
	if (CEventMgr::GetInst()->HasOccurObjEvent())
	{
		Renew();
	}
}

void ResourceUI::Reset()
{
	// Content 폴더 밑의 리소스 로딩
	Reload();

	Renew();
}

void ResourceUI::ItemClicked(void* pTreeNode)
{
	TreeNode*         pNode     = static_cast<TreeNode*>(pTreeNode);
	const std::string key       = pNode->GetName();
	CRes*             pResource = static_cast<CRes*>(pNode->GetData());
	// 프레임 노드가 눌렸다면 아무일도 없다.
	if (nullptr == pResource) return;

	m_pSelectedNode = pNode;
	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetResource(pResource);
}

void ResourceUI::ItemDBClicked(void* pTreeNode)
{
	const TreeNode* pNode     = static_cast<TreeNode*>(pTreeNode);
	const string    key       = pNode->GetName();
	CRes*           pResource = static_cast<CRes*>(pNode->GetData());
	// 프레임 노드가 눌렸다면 아무일도 없다.
	if (nullptr == pResource || pNode->GetParent()->GetName() != "SCENEFILE") return;

	// Scene 로딩, 현재 Scene 정지
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->SetSceneState(SCENE_STATE::STOP);

	// 로딩할 Scene 파일의 경로 계산
	std::wstring filePath = CPathMgr::GetInst()->GetContentPath();
	filePath += pResource->GetRelativePath();

	CScene* pNewScene = CSceneSaveLoad::LoadScene(filePath);
	CSceneMgr::GetInst()->GoToNextScene(pNewScene,false);
}


void ResourceUI::Reload()
{
	// Content 폴더 밑으로 모든 파일의 상대경로를 알아낸다.
	m_vecResPath.clear();
	FindFileName(CPathMgr::GetInst()->GetContentPath());

	// 키값 확인
	for (auto& keyAsPath : m_vecResPath)
	{
		// 1. 파일명을 통해서 리소스의 종류를 알아낸다.
		const RES_TYPE type = GetResTypeFromExt(keyAsPath);

		// 2. 상대경로를 키값으로 해서 리소스매니저에 로딩한다.
		switch (type)
		{
		case RES_TYPE::PREFAB:
			CResMgr::GetInst()->Load<CPrefab>(keyAsPath, keyAsPath);
			break;
		case RES_TYPE::MESHDATA:
			break;
		case RES_TYPE::MATERIAL:
			CResMgr::GetInst()->Load<CMaterial>(keyAsPath, keyAsPath);
			break;
		case RES_TYPE::MESH:
			break;
		case RES_TYPE::TEXTURE:
			CResMgr::GetInst()->Load<CTexture>(keyAsPath, keyAsPath);
			break;
		case RES_TYPE::SOUND:
			CResMgr::GetInst()->Load<CSound>(keyAsPath, keyAsPath);
			break;
		case RES_TYPE::SCENEFILE:
			CResMgr::GetInst()->Load<CSceneFile>(keyAsPath, keyAsPath);
			break;
		}
	}

	// 3. 로딩되어있는 리소스들의 실제 파일 존재 확인
	for (UINT i = 0; i < static_cast<UINT>(RES_TYPE::END); ++i)
	{
		const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(static_cast<RES_TYPE>(i));
		for (const auto& [key, pResource] : mapRes)
		{
			if (pResource->IsEngineRes()) continue;

			// File Exist 체크
			if (false == std::filesystem::exists(contentPath + pResource->GetRelativePath()))
			{
				// 4. 없으면 리소스매니저에서 메모리 해제
				MessageBox(nullptr, L"원본파일 삭제 됨", L"파일변경 감지", MB_OK);

				if (0 == pResource->GetRefCount())
				{
					// 삭제
					tEventInfo info{};
					info.eType  = EVENT_TYPE::DELETE_RES;
					info.lParam = reinterpret_cast<DWORD_PTR>(pResource);
					CEventMgr::GetInst()->AddEvent(info);

					MessageBox(nullptr, L"리소스 메모리 해제", L"파일변경 감지", MB_OK);
				}
				else
				{
					MessageBox(nullptr, L"사용 중인 리소스\n 메모리 해제 실패", L"파일변경 감지", MB_OK);
				}
			}
		}
	}
}

void ResourceUI::Renew()
{
	m_TreeUI->Clear();

	// 리소스를 Tree 에 표시
	for (int i = 0; i < static_cast<int>(RES_TYPE::END); ++i)
	{
		// TreeUI 가 DummyRoot 를 사용하기 때문에, 리소스 항목 노드들은 더미 자식으로 들어감
		TreeNode* pResNode = m_TreeUI->AddTreeNode(nullptr, ToString(static_cast<RES_TYPE>(i)));

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(static_cast<RES_TYPE>(i));

		for (const auto& [key, pResource] : mapRes)
		{
			// 각 리소스 노드들은 해당 리소스 항목 자식으로 들어감
			m_TreeUI->AddTreeNode(pResNode, ToString(key), pResource);
		}
	}
}

void ResourceUI::FindFileName(const wstring& folderPath)
{
	const std::wstring content = folderPath + L"*.*";

	WIN32_FIND_DATA FindFileData{};

	HANDLE hFind = nullptr;

	hFind = FindFirstFile(content.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return;

	while (FindNextFile(hFind, &FindFileData))
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (0 == wcscmp(FindFileData.cFileName, L".."))
				continue;

			FindFileName(folderPath + FindFileData.cFileName + L"\\");
			continue;
		}
		std::wstring relativePath = folderPath + FindFileData.cFileName;
		relativePath              = CPathMgr::GetInst()->GetRelativePath(relativePath);
		m_vecResPath.push_back(relativePath);
	}
	FindClose(hFind);
}


RES_TYPE ResourceUI::GetResTypeFromExt(const wstring& filePath)
{
	//wchar_t szExt[50]{};
	//_wsplitpath_s(filePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	const std::wstring extension = std::filesystem::path{filePath}.extension();

	if (L".mtrl" == extension) return RES_TYPE::MATERIAL;
	if (L".pref" == extension) return RES_TYPE::PREFAB;
	if (L".mesh" == extension) return RES_TYPE::MESH;
	if (L".scene" == extension) return RES_TYPE::SCENEFILE;
	if (L".mp3" == extension || L".wav" == extension || L".ogg" == extension) return RES_TYPE::SOUND;
	if (L".png" == extension
	    || L".jpeg" == extension
	    || L".bmp" == extension
	    || L".jpg" == extension
	    || L".tga" == extension
	    || L".dds" == extension)
		return RES_TYPE::TEXTURE;

	return RES_TYPE::END;
}

void ResourceUI::DeleteResourceFile(const TreeNode* pTreeNode)
{
	const std::string  relativePath = pTreeNode->GetName();
	const std::wstring contentPath  = CPathMgr::GetInst()->GetContentPath();
	const std::wstring filePath     = contentPath + ToWString(relativePath);

	std::filesystem::remove(std::filesystem::path{filePath});
	m_pSelectedNode = nullptr;
}
