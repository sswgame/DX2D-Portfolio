#pragma once
#include "CComponent.h"

#include "CTexture.h"
#include "CAnimation2D.h"

class CAnimator2D :
	public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*               m_pCurAnim;
	bool                        m_bRepeat;

public:
	void        finalupdate() override;
	void        UpdateData() override;
	static void Clear();

public:
	CAnimation2D* FindAnim(const wstring& _strName);
	void          CreateAnim(const wstring& _strName
	                       , Ptr<CTexture>  _pAtlas
	                       , Vec2           _vBackgroundSizePixel
	                       , Vec2           _vLeftTopPixel
	                       , Vec2           _vSlicePixel
	                       , Vec2           _vStepPixel
	                       , float          _fDuration
	                       , int            _iFrameCount);
	void SetAnimation(const std::wstring& name, CAnimation2D* pAnimation);

	void Play(const wstring& _strName, bool _bRepeat);

	void SetLoop(bool enable) { m_bRepeat = enable; }
	bool IsLoop() const { return m_bRepeat; }

	CAnimation2D*                                GetCurrentAnim() const { return m_pCurAnim; }
	const std::map<std::wstring, CAnimation2D*>& GetAnimationList() const { return m_mapAnim; }

	void Stop();

	void RemoveAllAnimation()
	{
		m_pCurAnim = nullptr;
		Safe_Del_Map(m_mapAnim);
	}

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CAnimator2D)

public:
	CAnimator2D();
	CAnimator2D(const CAnimator2D& _origin);
	virtual ~CAnimator2D();
};
