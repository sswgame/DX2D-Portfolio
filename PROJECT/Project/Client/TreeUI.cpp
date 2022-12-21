#include "pch.h"
#include "TreeUI.h"

#include "ImGui/imgui_internal.h"

// ========
// TreeNode
// ========
TreeNode::TreeNode()
	:
	m_pTreeUI(nullptr)
  , m_pParent(nullptr)
  , m_bLeaf(true)
  , m_bSelected(false)
  , m_pData(nullptr) {}

TreeNode::TreeNode(const string& _strName, void* pData)
	:
	m_pTreeUI(nullptr)
  , m_pParent(nullptr)
  , m_bLeaf(true)
  , m_bSelected(false)
  , m_strName(_strName)
  , m_pData(pData) {}

TreeNode::~TreeNode()
{
	Safe_Del_Vec(m_vecChild);
}


void TreeNode::update()
{
	// update 작성
	// child update
	for (const auto& pChildTreeNode : m_vecChild)
	{
		pChildTreeNode->update();
	}
}

void TreeNode::render_update()
{
	// ImGuiTreeNodeFlags_	
	ImGuiTreeNodeFlags eFlag = 0;

	if (m_bLeaf)
		eFlag |= ImGuiTreeNodeFlags_Leaf;
	if (m_pTreeUI->m_bUseFrame && (nullptr == m_pParent || m_pParent->m_strName == "DummyRoot"))
		eFlag |= ImGuiTreeNodeFlags_Framed;
	if (m_bSelected)
		eFlag |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(m_strName.c_str(), eFlag))
	{
		// 노드의 클릭체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			m_pTreeUI->SetSelectedNode(this);
		}
		// 노드의 더블클릭체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_pTreeUI->SetDBClickedNode(this);
		}
		// 내부 또는 외부로 드래그 드롭을 사용 할 경우
		if (m_pTreeUI->m_bUseDragDropSelf || m_pTreeUI->m_bUseDragDropOuter)
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str()
				                        , &m_pData
				                        , sizeof(void*));
				ImGui::Text(m_strName.c_str());
				ImGui::EndDragDropSource();

				m_pTreeUI->m_pDragNode = this;
			}

			// 내부 드롭을 허용한 경우에만 드롭체크
			if (m_pTreeUI->m_bUseDragDropSelf)
			{
				if (ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* payload =
						ImGui::AcceptDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str());
					if (nullptr != payload)
					{
						//void* pData = nullptr;
						//memcpy(&pData, payload->Data, sizeof(void*));*/

						// Drop Node 를 알림
						m_pTreeUI->m_pDropNode = this;
					}

					ImGui::EndDragDropTarget();
				}
			}
		}
		for (const auto& pChildTreeNode : m_vecChild)
		{
			pChildTreeNode->render_update();
		}
		ImGui::TreePop();
	}
}


// ======
// TreeUI
// ======
TreeUI::TreeUI(bool _bDummyRoot)
	:
	UI("##TreeUI")
  , m_pRootNode(nullptr)
  , m_pSelectedNode(nullptr)
  , m_pDragNode(nullptr)
  , m_pDropNode(nullptr)
  , m_bUseDummyRoot(_bDummyRoot)
  , m_bShowDummy(false)
  , m_bUseFrame(false)
  , m_bUseDragDropSelf(false)
  , m_bUseDragDropOuter(false)
  , m_CFunc(nullptr)
  , m_DBCFunc(nullptr)
  , m_DADFunc(nullptr)
{
	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

TreeUI::~TreeUI()
{
	SAFE_DELETE(m_pRootNode);
}

void TreeUI::update()
{
	if (nullptr == m_pRootNode)
	{
		m_pSelectedNode = nullptr;
		return;
	}

	// 트리UI 가 부착된 부모 UI 의 사이즈를 받아온다.
	Vec2 vSize = GetParentUI()->GetSize();
	vSize.y -= 39.f;
	SetSize(vSize);

	m_pRootNode->update();
	// 자식 UI 업데이트
	UI::update();
}

void TreeUI::render_update()
{
	if (nullptr == m_pRootNode)
		return;

	if (m_bUseDummyRoot)
	{
		if (m_bShowDummy)
		{
			m_pRootNode->render_update();
		}
		else
		{
			for (const auto& pChildTreeNode : m_pRootNode->m_vecChild)
			{
				pChildTreeNode->render_update();
			}
		}
	}
	else
	{
		m_pRootNode->render_update();
	}

	// Drag Drop Check
	if ((m_pDragNode && m_pDropNode)
	    || m_pDragNode && KEY_AWAY(KEY::LBTN))
	{
		if (m_DADFunc)
		{
			if (nullptr == m_pDropNode)
				m_DADFunc(m_pDragNode->GetData(), nullptr);
			else
				m_DADFunc(m_pDragNode->GetData(), m_pDropNode->GetData());

			m_pDragNode = nullptr;
			m_pDropNode = nullptr;
		}
	}


	// KeyBinding 호출
	if (ImGui::IsWindowFocused())
	{
		for (auto& [key, pFunc] : m_vecKeyBind)
		{
			if (KEY_TAP(key))
			{
				pFunc(m_pSelectedNode);
			}
		}
	}
}

TreeNode* TreeUI::AddTreeNode(TreeNode* _pParentNode, const string& _strName, void* pData)
{
	TreeNode* pNewNode  = new TreeNode{_strName, pData};
	pNewNode->m_pTreeUI = this;

	// 부모를 지정함
	if (nullptr != _pParentNode)
	{
		_pParentNode->AddChild(pNewNode);
	}
	// 부모가 지정되지 않고
	else if (nullptr == _pParentNode)
	{
		// 루트노드가 존재함
		if (nullptr != m_pRootNode)
			m_pRootNode->AddChild(pNewNode);
			// 루트노드가 존재하지 않음
		else
			m_pRootNode = pNewNode;
	}
	return pNewNode;
}

void TreeUI::Clear()
{
	SAFE_DELETE(m_pRootNode);
	m_pRootNode     = nullptr;
	m_pSelectedNode = nullptr;

	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

void TreeUI::CheckDragDrop()
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload =
			ImGui::AcceptDragDropPayload(GetAcceptDragUIName().c_str());
		if (nullptr != payload)
		{
			void* pData = nullptr;
			memcpy(&pData, payload->Data, sizeof(void*));

			if (m_dragDropFuc)
			{
				m_dragDropFuc(pData);
			}
		}

		ImGui::EndDragDropTarget();
	}
}

void TreeUI::SetSelectedNode(TreeNode* _pNode)
{
	if (nullptr != m_pSelectedNode)
	{
		m_pSelectedNode->m_bSelected = false;
	}

	m_pSelectedNode              = _pNode;
	m_pSelectedNode->m_bSelected = true;

	// 델리게이트 호출
	if (nullptr != m_CFunc)
	{
		m_CFunc(m_pSelectedNode);
	}
}

void TreeUI::SetDBClickedNode(TreeNode* _pNode) const
{
	// 델리게이트 호출
	if (nullptr != m_DBCFunc)
	{
		m_CFunc(m_pSelectedNode);
	}
}
