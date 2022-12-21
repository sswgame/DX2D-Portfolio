#pragma once
#include <Engine/CScript.h>

enum class SHAKE_DIR
{
	UP,
	DOWN,
	RIGHT,
	LEFT,

	END
};

class CCameraScript final
	: public CScript
{
public:
	CCameraScript();
	virtual ~CCameraScript();
	CLONE(CCameraScript);

public:
	void start() override;
	void update() override;

public:
	Vec2 GetLeftTop();

	void ShakeCamera(float time);

private:
	void SetBound();
	void ShakeCamera(Vec3& pos);
	void UpdatePosition();
private:
	CGameObject* m_pPlayer;
	float        m_shakeTime;
};
