#pragma once
#include <Engine/CScript.h>


class CFadeEffectScript;

class CStageScript final
	: public CScript
{
public:
	CStageScript();
	CStageScript(const CStageScript& other);
	virtual ~CStageScript();
	CLONE(CStageScript);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
public:
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void SetPlayerSpawnPos(const Vec2& pos) { m_playerPos = pos; }
	void SetNextSceneName(const std::wstring& sceneName) { m_sceneFileName = sceneName; }

private:
	Vec2               m_playerPos;
	std::wstring       m_sceneFileName;
	CFadeEffectScript* m_pFadeScript;
};

