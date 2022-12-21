#pragma once
#include "UI.h"

#include <Engine/CKeyMgr.h>

class TreeUI;

class TreeNode
{
private:
	TreeUI*           m_pTreeUI;
	TreeNode*         m_pParent;
	vector<TreeNode*> m_vecChild;
	bool              m_bLeaf;
	bool              m_bSelected;

	string m_strName;
	void*  m_pData;


public:
	void update();
	void render_update();

public:
	void AddChild(TreeNode* _pChild)
	{
		_pChild->m_pParent = this;
		m_vecChild.push_back(_pChild);
		m_bLeaf = false;
	}

	const string& GetName() const { return m_strName; }
	void*         GetData() const { return m_pData; }
	TreeNode*     GetParent() const { return m_pParent; }


public:
	TreeNode();
	TreeNode(const string& _strName, void* pData);
	~TreeNode();

	friend class TreeUI;
};

struct tTreeKey
{
	KEY                              eKey;
	std::function<void(void* pData)> pFunc;
};

class TreeUI final
	: public UI
{
private:
	TreeNode* m_pRootNode;
	TreeNode* m_pSelectedNode;
	TreeNode* m_pDragNode;
	TreeNode* m_pDropNode;

	const bool m_bUseDummyRoot;
	bool       m_bShowDummy;
	bool       m_bUseFrame;

	bool m_bUseDragDropSelf;
	bool m_bUseDragDropOuter;

	std::function<void(void* pData)>              m_CFunc;		// Clicked Delegate
	std::function<void(void* pData)>              m_DBCFunc;	// Double Clicked
	std::function<void(void* pDrag, void* pDrop)> m_DADFunc;	// Drag And Drop

	std::function<void(void* pDraggedNode)> m_dragDropFuc;
	// Key Binding Delegate
	vector<tTreeKey> m_vecKeyBind;

public:
	void update() override;
	void render_update() override;

public:
	void SetDragDropFunc(std::function<void(void* pDraggedNode)> func) { m_dragDropFuc = std::move(func); }

	void      ShowDummyRoot(bool _bTrue) { m_bShowDummy = _bTrue; }
	void      UseFrame(bool _b) { m_bUseFrame = _b; }
	void      UseDragDropOuter(bool _b) { m_bUseDragDropOuter = _b; }
	void      UseDragDropSelf(bool _b) { m_bUseDragDropSelf = _b; }
	TreeNode* AddTreeNode(TreeNode* _pParentNode, const string& _strName, void* pData = nullptr);
	TreeNode* GetDummyNode() const { return m_pRootNode; }

	void SetClickedDelegate(std::function<void(void* pData)> _Func) { m_CFunc = _Func; }
	void SetDoubleClickedDelegate(std::function<void(void* pData)> _Func) { m_DBCFunc = _Func; }
	void SetDragAndDropDelegate(std::function<void(void* pDrag, void* pDrop)> _Func) { m_DADFunc = _Func; }
	void SetKeyBinding(KEY _eKey, std::function<void(void* pData)> _Func) { m_vecKeyBind.push_back({_eKey, _Func}); }

	void Clear();

	TreeNode* GetDragNode() const { return m_pDragNode; }
	TreeNode* GetDropNode() const { return m_pDropNode; }

	void CheckDragDrop() override;
private:
	void SetSelectedNode(TreeNode* _pNode);
	void SetDBClickedNode(TreeNode* _pNode) const;

public:
	explicit TreeUI(bool _bDummyRoot);
	virtual  ~TreeUI();

	friend class TreeNode;
};
