#pragma once
#include "UI.h"

class TreeNode;
class TreeUI;

class ResourceUI :
	public UI
{
private:
	TreeUI*         m_TreeUI;
	TreeNode*       m_pSelectedNode;
	vector<wstring> m_vecResPath;

public:
	void update() override;
	void render_update() override;

public:
	void Reset();


private:
	void Reload();
	void Renew();
	void FindFileName(const wstring& folderPath);

	void ItemClicked(void* pTreeNode);
	void ItemDBClicked(void* pTreeNode);

	RES_TYPE GetResTypeFromExt(const wstring& filePath);

	void DeleteResourceFile(const TreeNode* pTreeNode);
public:
	ResourceUI();
	virtual ~ResourceUI();
};
