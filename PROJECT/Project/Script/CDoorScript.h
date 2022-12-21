#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

enum class DOOR_STATE
{
	OPEN,
	CLOSE,
	IDLE,
};

class CDoorScript final
	: public CScript
{
public:
	CDoorScript();
	virtual ~CDoorScript();
	CDoorScript(const CDoorScript& other);
	CLONE(CDoorScript);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
public:
	void start() override;
	void lateupdate() override;
private:
	DOOR_STATE   m_state;
	PARTICLE_DIR m_direction;
	std::wstring m_keyPrefabEnterScene;
	std::wstring m_keyPrefabDoorEffect;
	std::wstring m_keyPrefabColliderTile;

	CGameObject* m_pColliderTile;

	float m_accTime;

	Vec2         m_playerPos;
	std::wstring m_sceneFileName;
};
