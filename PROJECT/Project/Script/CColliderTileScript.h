#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

struct tRect
{
	float left;
	float top;
	float right;
	float bottom;
};

class CColliderTileScript final
	: public CScript
{
public:
	CColliderTileScript();
	virtual ~CColliderTileScript();
	CLONE(CColliderTileScript);

public:
	void start() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;
	void OnCollision(CGameObject* _OtherObject) override;
	void OnCollisionExit(CGameObject* _OtherObject) override;

	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	TILE_TYPE GetTileType() const { return m_tileType; }
private:
	void SetOtherPosition(CGameObject* pGameObject);

private:
	TILE_TYPE      m_tileType;
	COLLISION_SIDE m_collisionSide;
};
