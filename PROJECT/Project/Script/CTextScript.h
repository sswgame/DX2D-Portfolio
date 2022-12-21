#pragma once

#include <Engine/CFontManager.h>
#include <Engine/CScript.h>

class CCameraScript;

class CTextScript final
	: public CScript
{
public:
	CTextScript();
	CTextScript(const CTextScript& other);
	virtual ~CTextScript();
	CLONE(CTextScript);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void start() override;
	void update() override;

public:
	void SetContent(const std::wstring& content) const { m_pTextInfo->content = content; }
	void SetColor(const Vec4& color) const { m_pTextInfo->color = color; }
	void SetScale(float scale) const { m_pTextInfo->scale = scale; }
	void SetRotation(float radian) const { m_pTextInfo->rotationAngle = radian; }
	void RemoveFromScene() const { CFontManager::GetInst()->Remove(m_pTextInfo); }
	void InsertToScene() const { CFontManager::GetInst()->Insert(m_pTextInfo); }
	void Clear();
	void SetOnlyOnce(bool enable) { m_setOnlyOnce = enable; }
private:
	TextInfo*      m_pTextInfo;
	CCameraScript* m_pCameraScript;
	bool           m_waitForOneFrame;

	bool m_isFirstTime;
	bool m_setOnlyOnce;
};
