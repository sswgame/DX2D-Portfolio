#include "pch.h"
#include "CAnimation2D.h"

#include "CAnimator2D.h"
#include "CTimeMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "Ptr.h"

namespace
{
	constexpr int ANIM_REGISTER_NUMBER = 10;
}
CAnimation2D::CAnimation2D()
	:
	m_pOwner(nullptr)
  , m_iCurFrmIdx(0)
  , m_fAccTime(false)
  , m_bFinish(false) {}

CAnimation2D::~CAnimation2D() {}

void CAnimation2D::finalupdate()
{
	if (m_bFinish)
		return;

	m_fAccTime += DT;

	if (m_vecFrm[m_iCurFrmIdx].fDuration < m_fAccTime)
	{
		m_fAccTime -= m_vecFrm[m_iCurFrmIdx].fDuration;

		if (m_vecFrm.size() - 1 <= m_iCurFrmIdx)
		{
			m_bFinish = true;
		}
		else
		{
			++m_iCurFrmIdx;
		}
	}
}

void CAnimation2D::UpdateData()
{
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::ANIM2D);

	tAnim2D info      = {};
	info.useAnim2D    = 1;
	info.Atlas_Width  = m_pAtlasTex->Width();
	info.Atlas_Height = m_pAtlasTex->Height();

	info.vBackgroundSize = m_backGroundSize;
	info.vLT             = m_vecFrm[m_iCurFrmIdx].vLT;
	info.vSlice          = m_vecFrm[m_iCurFrmIdx].vSlice;
	info.vOffset         = m_vecFrm[m_iCurFrmIdx].vOffset;

	pBuffer->SetData(&info, sizeof(tAnim2D));
	pBuffer->UpdateData();


	m_pAtlasTex->UpdateData(PS, ANIM_REGISTER_NUMBER);
}

void CAnimation2D::Create(Ptr<CTexture> _Atlas
                        , Vec2          _vBackgroundSizePixel
                        , Vec2          _vLT
                        , Vec2          _vSlice
                        , Vec2          _vStep
                        , float         _fDuration
                        , int           _iFrameCount)
{
	assert(_Atlas.Get());

	m_pAtlasTex = _Atlas;

	float fWidth  = m_pAtlasTex->Width();
	float fHeight = m_pAtlasTex->Height();

	// 픽셀 좌표를 0~1 UV 로 전환
	Vec2 vLT    = _vLT / Vec2(fWidth, fHeight);
	Vec2 vSlice = _vSlice / Vec2(fWidth, fHeight);
	Vec2 vStep  = _vStep / Vec2(fWidth, fHeight);

	m_backGroundSize = _vBackgroundSizePixel / Vec2(fWidth, fHeight);

	// 프레임 정보 생성
	for (int i = 0; i < _iFrameCount; ++i)
	{
		tAnim2DFrame frm = {};

		frm.vLT       = vLT + (vStep * static_cast<float>(i));
		frm.vSlice    = vSlice;
		frm.fDuration = _fDuration;

		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::Create(Ptr<CTexture>                    _pAtlas
                        , Vec2                             _vBackgroundSizePixel
                        , const std::vector<tAnim2DFrame>& _vecFrames)
{
	assert(_pAtlas.Get());

	m_pAtlasTex = _pAtlas;
	m_vecFrm    = _vecFrames;

	const float fWidth      = m_pAtlasTex->Width();
	const float fHeight     = m_pAtlasTex->Height();
	const Vec2  vResolution = Vec2{fWidth, fHeight};
	m_backGroundSize        = _vBackgroundSizePixel / vResolution;
}

void CAnimation2D::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	size_t i = m_vecFrm.size();
	fwrite(&i, sizeof(size_t), 1, _pFile);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrame), i, _pFile);
	fwrite(&m_backGroundSize, sizeof(Vec2), 1, _pFile);

	SaveResPtr(m_pAtlasTex, _pFile);
}

void CAnimation2D::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	size_t i = 0;
	fread(&i, sizeof(size_t), 1, _pFile);
	m_vecFrm.resize(i);
	fread(m_vecFrm.data(), sizeof(tAnim2DFrame), i, _pFile);
	fread(&m_backGroundSize, sizeof(Vec2), 1, _pFile);

	LoadResPtr(m_pAtlasTex, _pFile);
}
