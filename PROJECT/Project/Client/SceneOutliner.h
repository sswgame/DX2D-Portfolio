#pragma once
#include "UI.h"

#include "TreeUI.h"

class CComponent;
class CGameObject;


class SceneOutLiner :
	public UI
{
private:
	TreeUI*     m_TreeUI;
	TreeNode*   m_pSelectedTarget;
	bool        m_isOpenPopUp;
	bool        m_isComponentAlreadyExist;
	std::string m_message;
	ImVec2      m_mousePos;

	bool m_isChangeName;
	bool m_checkExistingPrefab;
	bool m_isDeleteComponent;
	bool m_isDeleteScript;
public:
	void update() override;
	void render_update() override;

public:
	void Reset();
	void ObjectClicked(void* pTreeNode);

	void AddGameObjectToTree(CGameObject* _pObject, TreeNode* _pDestNode);
	void PressDelete(void* pTreeNode);
	void DragAndDropDelegate(void* pDragGameObject, void* pDropGameObject);
	void InstanciatePrefab(void* pDragPrefab);
	void DeSelect();
	bool CheckComponent(CComponent* pComponent, CGameObject* pGameObject);
	
private:
	std::wstring GetUniqueName(const std::wstring& name);
	void         AddToChild(TreeNode* pSelected);
	void         AddToOutLiner();

	void AddComponent(TreeNode* pSelected);
	void SelectComponent(void* pComponentName);
	void DeleteComponent(TreeNode* pSelected);

	void AddScript(TreeNode* pSelected);
	void SelectScript(void* pScriptName);
	void DeleteScript(TreeNode* pSelected);

	void         MakePrefab(const TreeNode* pSelected);
	Ptr<CPrefab> FindExistingPrefab(const TreeNode* pSelected);
public:
	SceneOutLiner();
	virtual ~SceneOutLiner();
};





