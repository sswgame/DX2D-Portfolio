#pragma once

#include "CEntity.h"

#include "CTexture.h"

class CAnimator2D;

class CAnimation2D
	: public CEntity
{
private:
	vector<tAnim2DFrame> m_vecFrm;
	Ptr<CTexture>        m_pAtlasTex;
	Vec2                 m_backGroundSize;

	CAnimator2D* m_pOwner;
	int          m_iCurFrmIdx;
	float        m_fAccTime;
	bool         m_bFinish;


public:
	void          ResetFrame(const std::vector<tAnim2DFrame>& vecFrames) { m_vecFrm = vecFrames; }
	tAnim2DFrame& GetFrame(int frameIndex) { return m_vecFrm[frameIndex]; }
	UINT          GetMaxFrame() const { return static_cast<UINT>(m_vecFrm.size()); }

	int  GetCurrentFrameIndex() const { return m_iCurFrmIdx; }

	void SetCurrentIndex(int index)
	{
		assert(0<=m_iCurFrmIdx && m_iCurFrmIdx<m_vecFrm.size());
		m_iCurFrmIdx = index;
	}

	Ptr<CTexture> GetAtlas() const { return m_pAtlasTex; }
	void          SetAtlas(Ptr<CTexture> pAtlas) { m_pAtlasTex = pAtlas; }

	Vec2          GetBackgroundSize() const { return m_backGroundSize; }
	void          SetBackgroundSize(Vec2 size) { m_backGroundSize = size; }


public:
	void finalupdate();

	void UpdateData() override;

public:
	void Create(Ptr<CTexture> _Atlas
	          , Vec2          _vBackgroundSizePixel
	          , Vec2          _vLT
	          , Vec2          _vSlice
	          , Vec2          _vStep
	          , float         _fDuration
	          , int           _iFrameCount);

	//주의 : _vecFrames tAnim2DFrame 정보는 보간된 값으로 세팅되어야 합니다.
	void Create(Ptr<CTexture> _pAtlas, Vec2 _vBackgroundSizePixel, const std::vector<tAnim2DFrame>& _vecFrames);

	bool IsFinish() const { return m_bFinish; }
	void SetFinish(bool bFinish) { m_bFinish = bFinish; }

	void Reset()
	{
		m_fAccTime   = 0.f;
		m_bFinish    = false;
		m_iCurFrmIdx = 0;
	}


public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CAnimation2D)

public:
	CAnimation2D();
	virtual ~CAnimation2D();

	friend class CAnimator2D;
};
