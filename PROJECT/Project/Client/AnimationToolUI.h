#pragma once
#include "TreeUI.h"
#include "UI.h"

struct SpriteInfo
{
	std::string name;
	int         index;
	ImVec2      position;
	ImVec2      scale;
	ImVec2      backGroundSize;
	float       offsetX;
	float       offsetY;
	float       duration;
};

class AnimationToolUI final
	: public UI
{
public:
	AnimationToolUI();
	virtual ~AnimationToolUI();

public:
	void render_update() override;

public:
	void SetTarget(CGameObject* pTarget);

private:
	void          OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath);
	Ptr<CTexture> LoadTexture() const;

	//CGameObject
	void ApplyToTarget();

	//Menu
	void DrawMenu();
	void New();

	//Load MetaFile or .Anim
	void Load();
	void LoadFromMeta();
	void LoadFromAnim();

	void Save();

	//Image Screen
	void DrawImage();
	void CropImageByDrag();
	void ShowBackGroundSize();

	//Animation Frames
	void DrawFrames();
	//Animation Hierarchy
	void DrawHierarchy();
	//Sprite Inspector
	void DrawSpriteInspector();
	void ChangeCheckAllBackgroundSize();
	void ItemClicked(void* pSelectedTreeNode);
	void Append();
	void Modify();
	void Remove();
	void Reverse();
	void TreeRenew();
	//Modify Screen
	void DrawModifyScreen();
	//Preview Screen
	void DrawPreviewScreen();
	void PlayPreview();
	void StopPreview();
	void UpdatePlayTime();

	void PlayAnimReset()
	{
		m_fAccTime   = 0.f;
		m_bFinish    = false;
		m_iCurFrmIdx = 0;
	}

	void DragAndDropDelegate(void* pDragData, void* pDropData);
	void Reset();
private:
	std::wstring m_filePath;

	Ptr<CTexture> m_pTexture2D;
	SpriteInfo    m_spriteInfo;

	ImVec2 m_startPos;
	ImVec2 m_lastPos;
	int    m_leastPadding;

	TreeUI*   m_pAnimTree;
	TreeNode* m_pSelectedNode;

	std::map<std::string, std::vector<SpriteInfo>> m_mapAnimations;

	bool m_isDrawing;
	bool m_applyReverseOrder;

	ImVec2 m_prevBackgroundSize;
	ImVec2 m_backgroundLeftTop;
	ImVec2 m_backgroundRightBottom;

	bool  m_isPlaying;
	float m_fAccTime;
	bool  m_bFinish;
	int   m_iCurFrmIdx;

	float m_leastBase;

	bool         m_repeat;
	CGameObject* m_pTarget;
};
