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

	// Clicked Delegate ���
	m_TreeUI->SetClickedDelegate(BIND_FN(ResourceUI::ItemClicked));
	// DoubleClicked Deletage ���
	m_TreeUI->SetDoubleClickedDelegate(BIND_FN(ResourceUI::ItemDBClicked));

	Reset();
}

ResourceUI::~ResourceUI() {}

void ResourceUI::update()
{
	// ���ҽ� ������� ����
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
	// Content ���� ���� ���ҽ� �ε�
	Reload();

	Renew();
}

void ResourceUI::ItemClicked(void* pTreeNode)
{
	TreeNode*         pNode     = static_cast<TreeNode*>(pTreeNode);
	const std::string key       = pNode->GetName();
	CRes*             pResource = static_cast<CRes*>(pNode->GetData());
	// ������ ��尡 ���ȴٸ� �ƹ��ϵ� ����.
	if (nullptr == pResource) return;

	m_pSelectedNode = pNode;
	// InspectorUI �� ����
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetResource(pResource);
}

void ResourceUI::ItemDBClicked(void* pTreeNode)
{
	const TreeNode* pNode     = static_cast<TreeNode*>(pTreeNode);
	const string    key       = pNode->GetName();
	CRes*           pResource = static_cast<CRes*>(pNode->GetData());
	// ������ ��尡 ���ȴٸ� �ƹ��ϵ� ����.
	if (nullptr == pResource || pNode->GetParent()->GetName() != "SCENEFILE") return;

	// Scene �ε�, ���� Scene ����
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->SetSceneState(SCENE_STATE::STOP);

	// �ε��� Scene ������ ��� ���
	std::wstring filePath = CPathMgr::GetInst()->GetContentPath();
	filePath += pResource->GetRelativePath();

	CScene* pNewScene = CSceneSaveLoad::LoadScene(filePath);
	CSceneMgr::GetInst()->GoToNextScene(pNewScene,false);
}


void ResourceUI::Reload()
{
	// Content ���� ������ ��� ������ ����θ� �˾Ƴ���.
	m_vecResPath.clear();
	FindFileName(CPathMgr::GetInst()->GetContentPath());

	// Ű�� Ȯ��
	for (auto& keyAsPath : m_vecResPath)
	{
		// 1. ���ϸ��� ���ؼ� ���ҽ��� ������ �˾Ƴ���.
		const RES_TYPE type = GetResTypeFromExt(keyAsPath);

		// 2. ����θ� Ű������ �ؼ� ���ҽ��Ŵ����� �ε��Ѵ�.
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

	// 3. �ε��Ǿ��ִ� ���ҽ����� ���� ���� ���� Ȯ��
	for (UINT i = 0; i < static_cast<UINT>(RES_TYPE::END); ++i)
	{
		const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(static_cast<RES_TYPE>(i));
		for (const auto& [key, pResource] : mapRes)
		{
			if (pResource->IsEngineRes()) continue;

			// File Exist üũ
			if (false == std::filesystem::exists(contentPath + pResource->GetRelativePath()))
			{
				// 4. ������ ���ҽ��Ŵ������� �޸� ����
				MessageBox(nullptr, L"�������� ���� ��", L"���Ϻ��� ����", MB_OK);

				if (0 == pResource->GetRefCount())
				{
					// ����
					tEventInfo info{};
					info.eType  = EVENT_TYPE::DELETE_RES;
					info.lParam = reinterpret_cast<DWORD_PTR>(pResource);
					CEventMgr::GetInst()->AddEvent(info);

					MessageBox(nullptr, L"���ҽ� �޸� ����", L"���Ϻ��� ����", MB_OK);
				}
				else
				{
					MessageBox(nullptr, L"��� ���� ���ҽ�\n �޸� ���� ����", L"���Ϻ��� ����", MB_OK);
				}
			}
		}
	}
}

void ResourceUI::Renew()
{
	m_TreeUI->Clear();

	// ���ҽ��� Tree �� ǥ��
	for (int i = 0; i < static_cast<int>(RES_TYPE::END); ++i)
	{
		// TreeUI �� DummyRoot �� ����ϱ� ������, ���ҽ� �׸� ������ ���� �ڽ����� ��
		TreeNode* pResNode = m_TreeUI->AddTreeNode(nullptr, ToString(static_cast<RES_TYPE>(i)));

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(static_cast<RES_TYPE>(i));

		for (const auto& [key, pResource] : mapRes)
		{
			// �� ���ҽ� ������ �ش� ���ҽ� �׸� �ڽ����� ��
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
