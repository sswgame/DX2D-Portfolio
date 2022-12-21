#pragma once
#include "UI.h"

struct ImageInfo
{
	std::string name;
	ImVec2      leftTop;
	ImVec2      scale;
	ImVec2      offset;
	ImVec2      backgroundSize;
};

class TextureToolUI final
	: public UI
{
public:
	TextureToolUI();
	virtual ~TextureToolUI();

public:
	void render_update() override;

	void SetTarget(CGameObject* pGameObject);

private:
	void          OpenFileDialog(const wchar_t* filterFormat, int filterIndex, const std::wstring& relativePath);
	Ptr<CTexture> LoadTexture() const;

	//CGameObject
	void ApplyToTarget();

	//Load MetaFile
	void GetTexture();
	void LoadMetaInfo();

	//Settings for ImGui Drawing
	void SetZoomScale();
	void Reset();
	//Draw ImGui
	void DrawMenu();
	void DrawImage();
	void DrawImageButtonOnSprite();
	void DrawImageAsWholeSprite();
	void DrawSelectedImageInfo();
private:
	std::wstring m_filePath;

	Ptr<CTexture> m_pTexture2D;
	CGameObject*  m_pTarget;

	ImageInfo              m_selectedImageInfo;
	float                  m_leastBase;
	std::vector<ImageInfo> m_vecImageInfo;

	float m_zoomScale;
	bool  m_isSelected;
	bool  m_isOpenWarningPopUP;
	bool  m_hasMetaInfo;
};
