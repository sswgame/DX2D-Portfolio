#pragma once
#include "ImGui/imgui.h"

inline const wchar_t*  TEXTURE_FILTER_FORMAT   = L"Texture\0*.bmp;*.png;*.jpeg;*.jpg;*.dds;*.tga\0All\0*.*\0";
inline const wchar_t*  ANIM_META_FILTER_FORMAT = L"Anim\0*.anim\0UnityMeta\0*.txt\0";
inline const wchar_t*  TXT_FILTER_FORMAT       = L"Text\0*.txt\0";
inline const wchar_t*  TILE_FILTER_FORMAT      = L"Tile\0*.tile\0";
inline const wchar_t*  ANIM_FILTER_FORMAT      = L"Anim\0*.anim\0";
inline const wchar_t*  XML_FILTER_FORMAT       = L"xml\0*.XML\0";
inline constexpr float HEIGHT_PADDING          = 60.f;

#define BIND_FN(fn) [this](auto&&...args){this->fn(std::forward<decltype(args)>(args)...);}

class UI
{
private:
	const string m_name;
	string       m_title;
	vector<UI*>  m_vecChildUI;
	UI*          m_pParentUI;

	Vec2 m_vPos;
	Vec2 m_vSize;
	bool m_bOpen;
	bool m_bModal;

	//윈도우 플래그 세팅
	int m_windowFlags;

	//드래그드롭 대상
	std::string m_acceptDragDropUIName;
public:
	std::string GetAcceptDragUIName() const { return m_acceptDragDropUIName; }
	void        SetAcceptDragUIName(const std::string& name) { m_acceptDragDropUIName = name; }

	void        SetImGuiWindowFlags(int flag) { m_windowFlags = flag; }

	const string& GetName() const { return m_name; }
	void          SetSize(Vec2 _vSize) { m_vSize = _vSize; }
	Vec2          GetSize() const { return m_vSize; }

	void SetPos(Vec2 _vPos) { m_vPos = _vPos; }
	Vec2 GetPos() const { return m_vPos; }

	void          SetTitle(const string& _strTitle) { m_title = _strTitle; }
	const string& GetTitle() { return m_title; }

	void SetModalUI(bool _bTrue) { m_bModal = _bTrue; }
	bool IsModal() const { return m_bModal; }

	UI* GetParentUI() const { return m_pParentUI; }

	void AddChild(UI* _pChildUI)
	{
		_pChildUI->m_pParentUI = this;
		m_vecChildUI.push_back(_pChildUI);
	}

	virtual void Activate() { m_bOpen = true; }
	virtual void Deactivate() { m_bOpen = false; };

	bool IsActive() const { return m_bOpen; }

	virtual void CheckDragDrop() {};
public:
	virtual void update();
	virtual void render();
	virtual void render_update() = 0;

public:
	explicit UI(const string& _strName);
	virtual ~UI();
};
