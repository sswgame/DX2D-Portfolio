#include "pch.h"
#include "SceneOutLiner.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CEventMgr.h>
#include <Engine/CScript.h>

#include <Script/CSceneSaveLoad.h>

#include "TreeUI.h"
#include "CImGuiMgr.h"
#include "InspectorUI.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ListUI.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_stdlib.h"

SceneOutLiner::SceneOutLiner()
	:
	UI("SceneOutLiner")
  , m_TreeUI{nullptr}
  , m_pSelectedTarget{nullptr}
  , m_isOpenPopUp{false}
  , m_isComponentAlreadyExist{false}
  , m_isChangeName{false}
  , m_checkExistingPrefab{false}
  , m_isDeleteComponent{false}
  , m_isDeleteScript{false}
{
	m_TreeUI = new TreeUI(true);
	m_TreeUI->SetTitle("SceneOutLiner");
	m_TreeUI->UseFrame(false);
	m_TreeUI->ShowDummyRoot(false);
	m_TreeUI->UseDragDropSelf(true);
	m_TreeUI->UseDragDropOuter(true);
	m_TreeUI->SetDragDropFunc(BIND_FN(SceneOutLiner::InstanciatePrefab));

	m_TreeUI->SetAcceptDragUIName("Resource");
	AddChild(m_TreeUI);
	// Clicked Delegate 등록
	m_TreeUI->SetClickedDelegate(BIND_FN(SceneOutLiner::ObjectClicked));
	// Drag and Drop Delegate 등록
	m_TreeUI->SetDragAndDropDelegate(BIND_FN(SceneOutLiner::DragAndDropDelegate));
	// Key Delegate 등록
	m_TreeUI->SetKeyBinding(KEY::DEL, BIND_FN(SceneOutLiner::PressDelete));

	Reset();
}

SceneOutLiner::~SceneOutLiner() {}

void SceneOutLiner::update()
{
	if (CEventMgr::GetInst()->HasOccurObjEvent())
	{
		Reset();
	}

	UI::update();
}

void SceneOutLiner::render_update()
{
	//마우스 우측 클릭으로 마우스 위치에 팝업 만들기
	const ImVec2   windowLeftTop = ImGui::GetCursorScreenPos();
	const ImVec2   windowSize    = ImGui::GetContentRegionAvail();
	const ImGuiIO& io            = ImGui::GetIO();
	const ImVec2   mousePos      = io.MousePos - windowLeftTop;

	if (0 < mousePos.x && mousePos.x < windowSize.x
	    && 0 < mousePos.y && mousePos.y < windowSize.y)
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("PREFAB_POPUP");
			m_isOpenPopUp = true;
		}
	}
	if (m_isComponentAlreadyExist)
	{
		ImGui::SetNextWindowPos(m_mousePos);
		ImGui::OpenPopup("COMPONENT EXIST");
		if (ImGui::BeginPopupModal("COMPONENT EXIST", &m_isComponentAlreadyExist))
		{
			ImGui::Text(m_message.c_str());
			ImGui::AlignTextToFramePadding();
			if (ImGui::Button("Close"))
			{
				m_isComponentAlreadyExist = false;
			}
			ImGui::EndPopup();
		}
	}
	if (m_isOpenPopUp)
	{
		static const std::string menus[] = {
			"Create"
		  , "Delete"
		  , "Add Component"
		  , "Delete Component"
		  , "Add Script"
		  , "Delete Script"
		  , "Make as Prefab"
		  , "Change Name"
		};
		if (ImGui::BeginPopup("PREFAB_POPUP"))
		{
			for (int i = 0; i < std::size(menus); ++i)
			{
				if (ImGui::Selectable(menus[i].c_str()))
				{
					if ("Delete" == menus[i] && m_pSelectedTarget)
						PressDelete(m_pSelectedTarget);
					else if ("Create" == menus[i] && nullptr == m_pSelectedTarget)
						AddToOutLiner();
					else if ("Create" == menus[i] && nullptr != m_pSelectedTarget)
						AddToChild(m_pSelectedTarget);
					else if ("Add Component" == menus[i] && nullptr != m_pSelectedTarget)
						AddComponent(m_pSelectedTarget);
					else if ("Add Script" == menus[i] && nullptr != m_pSelectedTarget)
						AddScript(m_pSelectedTarget);
					else if ("Change Name" == menus[i] && nullptr != m_pSelectedTarget)
					{
						m_isChangeName = true;
					}
					else if ("Make as Prefab" == menus[i] && nullptr != m_pSelectedTarget)
					{
						if (nullptr == FindExistingPrefab(m_pSelectedTarget))
							MakePrefab(m_pSelectedTarget);
						else
							m_checkExistingPrefab = true;
					}
					else if ("Delete Component" == menus[i] && nullptr != m_pSelectedTarget)
					{
						DeleteComponent(m_pSelectedTarget);
					}
					else if ("Delete Script" == menus[i] && nullptr != m_pSelectedTarget)
					{
						DeleteScript(m_pSelectedTarget);
					}
				}
			}
			ImGui::EndPopup();
		}
	}
	if (m_isChangeName && nullptr != m_pSelectedTarget)
	{
		ImGui::OpenPopup("CHANGE NAME");
		if (ImGui::BeginPopupModal("CHANGE NAME", &m_isChangeName))
		{
			CGameObject*      pGameObject = static_cast<CGameObject*>(m_pSelectedTarget->GetData());
			const std::string oldName     = ToString(pGameObject->GetName());
			static bool       isTyping    = false;

			static std::string name{};
			if (false == isTyping)
			{
				name = (oldName.empty()) ? "NO_NAME" : oldName;
			}

			CKeyMgr::GetInst()->SetEnableTyping(true);
			if (ImGui::InputText("##LAYER_NAME"
			                   , &name
			                   , ImGuiInputTextFlags_CharsNoBlank))
			{
				isTyping = true;
			}

			if (ImGui::Button("APPLY") || KEY_TAP(KEY::ENTER))
			{
				pGameObject->SetName(GetUniqueName(ToWString(name)));
				CKeyMgr::GetInst()->SetEnableTyping(false);
				isTyping          = false;
				m_isChangeName    = false;
				m_pSelectedTarget = nullptr;
				ImGui::CloseCurrentPopup();
				Reset();
			}
			ImGui::EndPopup();
		}
	}

	if (m_checkExistingPrefab && nullptr != m_pSelectedTarget)
	{
		ImGui::OpenPopup("EXISTING PREFAB_FOUND");
		if (ImGui::BeginPopupModal("EXISTING PREFAB_FOUND"))
		{
			ImGui::Text("EXISTING PREFAB FOUND! YOU CAN NOT MAKE IT AS PREFAB");
			if (ImGui::Button("CLOSE"))
			{
				m_checkExistingPrefab = false;
				m_pSelectedTarget     = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	if (KEY_TAP(KEY::LBTN) && KEY_PRESSED(KEY::LCTRL))
	{
		DeSelect();
	}

	if (KEY_TAP(KEY::F2) && nullptr != m_pSelectedTarget)
	{
		m_isChangeName = true;
	}
}

Ptr<CPrefab> SceneOutLiner::FindExistingPrefab(const TreeNode* pSelected)
{
	const CGameObject* pGameObject = static_cast<CGameObject*>(pSelected->GetData());
	std::wstring       keyAsPath   = (std::filesystem::path{L"prefab"} / pGameObject->GetName()).c_str();
	keyAsPath += L".pref";

	return CResMgr::GetInst()->FindRes<CPrefab>(keyAsPath);
}

void SceneOutLiner::MakePrefab(const TreeNode* pSelected)
{
	CGameObject* pGameObject = static_cast<CGameObject*>(pSelected->GetData());
	std::wstring keyAsPath   = (std::filesystem::path{L"prefab"} / pGameObject->GetName()).c_str();
	keyAsPath += L".pref";

	CPrefab* pPrefab = new CPrefab{};
	pPrefab->SetProto(pGameObject->Clone());

	CResMgr::GetInst()->AddRes<CPrefab>(keyAsPath, pPrefab);

	const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
	const std::wstring filePath    = contentPath + keyAsPath;
	CSceneSaveLoad::SavePrefab(pPrefab, filePath);
}

std::wstring SceneOutLiner::GetUniqueName(const std::wstring& name)
{
	auto vecAllGameObjects = CSceneMgr::GetInst()->GetAllObjects();

	const size_t count = std::count_if(vecAllGameObjects.begin()
	                                 , vecAllGameObjects.end()
	                                 , [name](const CGameObject* pGameObject)
	                                   {
		                                   const size_t pos = pGameObject->GetName().find(name);
		                                   return pos != pGameObject->GetName().npos && pos == 0;
	                                   });
	return (0 == count || 1 == count) ? name : std::wstring{name} + L"_" + std::to_wstring(count);
}

void SceneOutLiner::AddToChild(TreeNode* pSelected)
{
	CGameObject* pNewGameObject = new CGameObject{};
	pNewGameObject->SetName(GetUniqueName(L"New GameObject"));
	pNewGameObject->AddComponent(new CTransform{});

	CGameObject* pParent = static_cast<CGameObject*>(pSelected->GetData());
	CSceneMgr::GetInst()->AddChild(pParent, pNewGameObject);

	AddGameObjectToTree(pNewGameObject, pSelected);

	m_pSelectedTarget = nullptr;
	m_isOpenPopUp     = false;
}

void SceneOutLiner::AddToOutLiner()
{
	CGameObject* pNewGameObject = new CGameObject{};
	pNewGameObject->SetName(GetUniqueName(L"New GameObject"));
	pNewGameObject->AddComponent(new CTransform{});

	CSceneMgr::GetInst()->SpawnObject(pNewGameObject, 0);

	AddGameObjectToTree(pNewGameObject, m_TreeUI->GetDummyNode());
	m_pSelectedTarget = nullptr;
	m_isOpenPopUp     = false;
}

void SceneOutLiner::AddComponent(TreeNode* pSelected)
{
	// ListUI 활성화한다.
	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	pListUI->Clear();
	pListUI->SetTitle("Component List");

	for (int i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		pListUI->AddList(ToString(static_cast<COMPONENT_TYPE>(i)));
	}
	m_pSelectedTarget = pSelected;

	pListUI->Activate();
	pListUI->SetDBCEvent(BIND_FN(SceneOutLiner::SelectComponent));
}

void SceneOutLiner::SelectComponent(void* pComponentName)
{
	const std::string  selectedName = static_cast<char*>(pComponentName);
	const std::wstring name         = ToWString(selectedName);
	CGameObject*       pGameObject  = static_cast<CGameObject*>(m_pSelectedTarget->GetData());

	if (m_isDeleteComponent)
	{
		COMPONENT_TYPE type = COMPONENT_TYPE::END;
		for (int i = 0; i < static_cast<int>(COMPONENT_TYPE::END); ++i)
		{
			if (ToWString(static_cast<COMPONENT_TYPE>(i)) == name)
			{
				type = static_cast<COMPONENT_TYPE>(i);
				break;
			}
		}
		pGameObject->DeleteComponent(type);
		m_isDeleteComponent = false;
	}
	else
	{
		CComponent* pComponent = CComponent::MakeComponent(name);
		if (false == CheckComponent(pComponent, pGameObject)) { return; }

		pGameObject->AddComponent(pComponent);
	}

	DeSelect();
}

void SceneOutLiner::DeleteComponent(TreeNode* pSelected)
{
	const CGameObject* pGameObject = static_cast<CGameObject*>(pSelected->GetData());

	// ListUI 활성화한다.
	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	pListUI->Clear();
	pListUI->SetTitle("Component List");

	for (int i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (nullptr != pGameObject->GetComponent(static_cast<COMPONENT_TYPE>(i))
		    && COMPONENT_TYPE::TRANSFORM != static_cast<COMPONENT_TYPE>(i))
		{
			pListUI->AddList(ToString(static_cast<COMPONENT_TYPE>(i)));
		}
	}
	m_pSelectedTarget   = pSelected;
	m_isDeleteComponent = true;
	pListUI->Activate();
	pListUI->SetDBCEvent(BIND_FN(SceneOutLiner::SelectComponent));
}

void SceneOutLiner::AddScript(TreeNode* pSelected)
{
	// ListUI 활성화한다.
	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	pListUI->Clear();
	pListUI->SetTitle("Script List");

	vector<wstring> vecScriptName{};
	CScriptMgr::GetScriptInfo(vecScriptName);
	for (auto& scriptName : vecScriptName)
	{
		pListUI->AddList(ToString(scriptName));
	}
	m_pSelectedTarget = pSelected;

	pListUI->Activate();
	pListUI->SetDBCEvent(BIND_FN(SceneOutLiner::SelectScript));
}

void SceneOutLiner::SelectScript(void* pScriptName)
{
	const std::string  scriptName  = static_cast<char*>(pScriptName);
	const std::wstring name        = ToWString(scriptName);
	CGameObject*       pGameObject = static_cast<CGameObject*>(m_pSelectedTarget->GetData());

	if (m_isDeleteScript)
	{
		const CScript* pScript = pGameObject->GetScriptByName(name);
		pGameObject->DeleteScript(pScript->GetScriptType());
		m_isDeleteScript = false;
	}
	else
	{
		CScript* pScript = CScriptMgr::GetScript(name);
		if (false == CheckComponent(pScript, pGameObject))
		{
			return;
		}
		pGameObject->AddComponent(pScript);
	}

	DeSelect();
}

void SceneOutLiner::DeleteScript(TreeNode* pSelected)
{
	CGameObject* pGameObject = static_cast<CGameObject*>(pSelected->GetData());

	// ListUI 활성화한다.
	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	pListUI->Clear();
	pListUI->SetTitle("Component List");

	auto& vecAllScripts = pGameObject->GetScripts();
	for (auto& pScript : vecAllScripts)
	{
		pListUI->AddList(ToString(CScriptMgr::GetScriptName(pScript)));
	}
	m_pSelectedTarget = pSelected;
	m_isDeleteScript  = true;
	pListUI->Activate();
	pListUI->SetDBCEvent(BIND_FN(SceneOutLiner::SelectScript));
}

void SceneOutLiner::Reset()
{
	m_TreeUI->Clear();

	// 현재 Scene 을 가져온다.
	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);

		vector<CGameObject*>& vecRoots = pLayer->GetRootObjects();

		for (const auto& pGameObject : vecRoots)
		{
			AddGameObjectToTree(pGameObject, m_TreeUI->GetDummyNode());
		}
	}

	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetObject(nullptr);
}

void SceneOutLiner::ObjectClicked(void* pTreeNode)
{
	TreeNode* pNode = static_cast<TreeNode*>(pTreeNode);

	string       strKey  = pNode->GetName();
	CGameObject* pObject = static_cast<CGameObject*>(pNode->GetData());
	m_pSelectedTarget    = pNode;
	assert(pObject);

	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetObject(pObject);
}

void SceneOutLiner::AddGameObjectToTree(CGameObject* _pObject, TreeNode* _pDestNode)
{
	TreeNode* pNode = m_TreeUI->AddTreeNode(_pDestNode
	                                      , ToString(_pObject->GetName())
	                                      , _pObject);

	const vector<CGameObject*>& vecChild = _pObject->GetChild();

	for (auto& pChildGameObject : vecChild)
	{
		AddGameObjectToTree(pChildGameObject, pNode);
	}
}

void SceneOutLiner::PressDelete(void* pTreeNode)
{
	const TreeNode* pNode = static_cast<TreeNode*>(pTreeNode);
	if (nullptr == pNode) return;

	CGameObject* pTargetObj = static_cast<CGameObject*>(pNode->GetData());
	pTargetObj->Destroy();

	// InspectorUI 를 찾아서 Object 를 nullptr 로 세팅한다.
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetObject(nullptr);
	m_pSelectedTarget = nullptr;
}

void SceneOutLiner::DragAndDropDelegate(void* pDragGameObject, void* pDropGameObject)
{
	CGameObject* pChildObject      = static_cast<CGameObject*>(pDragGameObject);
	CGameObject* pDropTargetObject = static_cast<CGameObject*>(pDropGameObject);

	// 드롭 목적지가 제대로 들어 온 경우
	if (nullptr != pDropTargetObject)
	{
		if (pChildObject == pDropTargetObject || pDropTargetObject->IsAncestor(pChildObject)) { return; }

		CSceneMgr::GetInst()->AddChild(pDropTargetObject, pChildObject);
	}
	// 자식 오브젝트의 목적지가 nullptr 인 경우
	else
	{
		// 이미 최상위 부모 오브젝트는 이벤트 처리가 필요없다.
		if (nullptr == pChildObject->GetParent()) { return; }

		CSceneMgr::GetInst()->DisconnectParent(pChildObject);
	}
}

void SceneOutLiner::InstanciatePrefab(void* pDragPrefab)
{
	CEntity* pEntity = static_cast<CEntity*>(pDragPrefab);
	CPrefab* pPrefab = dynamic_cast<CPrefab*>(pEntity);

	if (nullptr != pPrefab)
	{
		const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
		const std::wstring filePath    = contentPath + pPrefab->GetRelativePath();
		CSceneSaveLoad::LoadPrefab(pPrefab, filePath);
		CGameObject* pNewGameObject = pPrefab->Instantiate();
		pNewGameObject->SetName(GetUniqueName(pNewGameObject->GetName()));

		CSceneMgr::GetInst()->SpawnObject(pNewGameObject, 0);

		AddGameObjectToTree(pNewGameObject, m_TreeUI->GetDummyNode());
	}
}

void SceneOutLiner::DeSelect()
{
	m_pSelectedTarget = nullptr;
	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspectorUI->SetTargetObject(nullptr);
}

bool SceneOutLiner::CheckComponent(CComponent* pComponent, CGameObject* pGameObject)
{
	switch (pComponent->GetType())
	{
	case COMPONENT_TYPE::TRANSFORM:
	case COMPONENT_TYPE::CAMERA:
	case COMPONENT_TYPE::COLLIDER2D:
	case COMPONENT_TYPE::ANIMATOR2D:
	case COMPONENT_TYPE::LIGHT2D:
		if (nullptr != pGameObject->GetComponent(pComponent->GetType()))
		{
			m_isComponentAlreadyExist = true;
			m_message                 = "COMPONENT ALREADY EXISTS!";
		}
		break;
	case COMPONENT_TYPE::SCRIPT:
		{
			CScript* pScript    = static_cast<CScript*>(pComponent);
			auto&    vecScripts = pGameObject->GetScripts();
			auto     iter
				= std::find_if(vecScripts.begin()
				             , vecScripts.end()
				             , [id = pScript->GetScriptType()](const CScript* pData)
				               {
					               return pData->GetScriptType() == id;
				               });
			if (iter != vecScripts.end())
			{
				m_isComponentAlreadyExist = true;
				m_message                 = "SCRIPT ALREADY EXISTS!";
			}
		}
		break;
	case COMPONENT_TYPE::MESHRENDER:
	case COMPONENT_TYPE::TILEMAP:
	case COMPONENT_TYPE::PARTICLESYSTEM:
		if (nullptr != pGameObject->GetRenderComponent())
		{
			m_isComponentAlreadyExist = true;
			m_message                 = "RENDER COMPONENT EXISTS!";
		}
		break;
	}

	if (m_isComponentAlreadyExist)
	{
		delete pComponent;
		m_mousePos = ImGui::GetMousePos();
		return false;
	}
	return true;
}
