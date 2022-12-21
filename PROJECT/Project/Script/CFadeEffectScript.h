#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

enum class FADE_TYPE
{
	FADE_IN,
	FADE_OUT,
	END
};
class CFadeEffectScript final
	: public CScript
{
public:
	CFadeEffectScript();
	virtual ~CFadeEffectScript();
	CLONE(CFadeEffectScript);

public:
	void start() override;
	void update() override;
	void UpdateData() override;;

public:
	void SetFadeType(FADE_TYPE type) { m_fadeType = type; }
	void SetTime(float time) { m_fadeTime = time; }

	void Reset();
	bool IsFinished() const { return m_isFinished; }
private:
	float     m_accTime;
	float     m_fadeTime;
	float     m_alpha;
	FADE_TYPE m_fadeType;

	bool m_isFinished;
};
