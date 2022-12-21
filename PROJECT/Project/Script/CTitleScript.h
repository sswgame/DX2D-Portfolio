#pragma once
#include <array>
#include <Engine/CScript.h>

class CMouseScript;

enum class TITLE_MENU
{
	PLAY,
	OPTION,
	EXIT,
	END
};

class CTitleScript final
	: public CScript
{
public:
	CTitleScript();
	virtual ~CTitleScript();
	CTitleScript(const CTitleScript& other);
	CLONE(CTitleScript);

public:
	void start() override;
	void update() override;

	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

private:
	void FindAllChild();
	void MoveCloud();
	void CheckInput();
	void DoAction(TITLE_MENU type);
	void UpdateMousePosition();

private:
	CGameObject* m_pFrontCloud;
	float        m_frontCloudSpeed;
	float        m_maxFrontPosX;
	float        m_startFrontPosX;

	CGameObject* m_pBackCloud;
	float        m_backCloudSpeed;
	float        m_maxBackCloudPosX;
	float        m_startBacCloudPosX;

	int                                                          m_selectIndex;
	bool                                                         m_isMouseMove;
	TITLE_MENU                                                   m_menuSelected;
	std::array<CGameObject*, static_cast<UINT>(TITLE_MENU::END)> m_arrMenuObjects;

	std::wstring m_nextSceneName;

	CMouseScript* m_pMouseScript;
};
