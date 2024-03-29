#include "pch.h"
#include "CCamera.h"

#include "CCollider2D.h"
#include "CDevice.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"

namespace
{
	constexpr int INVALID_CAMERA_INDEX = -1;
}
CCamera::CCamera()
	:
	CComponent(COMPONENT_TYPE::CAMERA)
  , m_eProjType(PROJ_TYPE::ORTHOGRAPHIC)
  , m_fWidth(0.f)
  , m_fAspectRatio(1.f)
  , m_fFOV(XM_PI / 4.f)
  , m_fFar(10000.f)
  , m_iLayerMask(0)
  , m_iCamIdx(INVALID_LAYER_INDEX)
{
	m_fWidth       = CDevice::GetInst()->GetRenderResolution().x;
	m_fAspectRatio = CDevice::GetInst()->GetRenderResolution().x / CDevice::GetInst()->GetRenderResolution().y;
}

CCamera::CCamera(const CCamera& _origin)
	:
	CComponent(_origin)
  , m_eProjType(_origin.m_eProjType)
  , m_fWidth(_origin.m_fWidth)
  , m_fAspectRatio(_origin.m_fAspectRatio)
  , m_fFOV(_origin.m_fFOV)
  , m_fFar(_origin.m_fFar)
  , m_iLayerMask(_origin.m_iLayerMask)
  , m_iCamIdx(-1) {}

CCamera::~CCamera() {}

void CCamera::finalupdate()
{
	// View 행렬 계산
	const Vec3 cameraPosition = Transform()->GetRelativePos();
	// View 이동행렬
	Matrix matViewTrans = XMMatrixTranslation(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);
	// View 회전행렬
	// Right, Up, Front 를 가져온다.
	Matrix matViewRot = XMMatrixIdentity();

	const Vec3 vRight = Transform()->GetWorldRightDir();
	const Vec3 vUp    = Transform()->GetWorldUpDir();
	const Vec3 vFront = Transform()->GetWorldFrontDir();

	matViewRot._11 = vRight.x;
	matViewRot._12 = vUp.x;
	matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y;
	matViewRot._22 = vUp.y;
	matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z;
	matViewRot._32 = vUp.z;
	matViewRot._33 = vFront.z;

	m_matView = matViewTrans * matViewRot;


	// 투영행렬 계산
	if (PROJ_TYPE::ORTHOGRAPHIC == m_eProjType)
	{
		const float height = m_fWidth / m_fAspectRatio;
		m_matProj          = XMMatrixOrthographicLH(m_fWidth, height, 0.f, 5000.f);
	}
	else
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, m_fAspectRatio, 1.f, m_fFar);
	}

	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::SortGameObject()
{
	m_vecForward.clear();
	m_vecMasked.clear();
	m_vecTranslucent.clear();
	m_vecPostProcess.clear();

	m_vecCollider.clear();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 카메라가 찍을 대상 레이어가 아니면 continue
		if (!(m_iLayerMask & (1 << i)))
			continue;

		CLayer*               pLayer = pCurScene->GetLayer(i);
		vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (auto& pGameObject : vecObj)
		{
			CRenderComponent* pRenderCom = pGameObject->GetRenderComponent();
			CCollider2D*      pCollider  = pGameObject->Collider2D();
			if (nullptr != pCollider)
			{
				m_vecCollider.push_back(pCollider);
			}

			if (nullptr == pRenderCom
			    || nullptr == pRenderCom->GetMesh()
			    || nullptr == pRenderCom->GetMaterial()
			    || nullptr == pRenderCom->GetMaterial()->GetShader())
			{
				continue;
			}

			Ptr<CGraphicsShader> pShader = pRenderCom->GetMaterial()->GetShader();

			switch (pShader->GetShaderDomain())
			{
			case SHADER_DOMAIN::DOMAIN_FORWARD:
				m_vecForward.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_MASKED:
				m_vecMasked.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_TRANSLUCENT:
				m_vecTranslucent.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(pGameObject);
				break;
			}
		}
	}
}

void CCamera::render_forward()
{
	for (size_t i = 0; i < m_vecForward.size(); ++i)
	{
		if (m_vecForward[i]->IsActive())
			m_vecForward[i]->render();
	}
}

void CCamera::render_masked()
{
	for (size_t i = 0; i < m_vecMasked.size(); ++i)
	{
		if (m_vecMasked[i]->IsActive())
			m_vecMasked[i]->render();
	}
}

void CCamera::render_translucent()
{
	for (size_t i = 0; i < m_vecTranslucent.size(); ++i)
	{
		if (m_vecTranslucent[i]->IsActive())
			m_vecTranslucent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		if (m_vecPostProcess[i]->IsActive())
		{
			CRenderMgr::GetInst()->CopyTargetToPostProcess();
			m_vecPostProcess[i]->render();
		}
	}
}

void CCamera::RenderCollider()
{
	if (KEY_TAP(KEY::F1))
	{
		static bool isDraw = false;
		isDraw             = !isDraw;
		CSceneMgr::GetInst()->RenderCollider(isDraw);
	}

	if (false == CSceneMgr::GetInst()->IsDrawCollider()) { return; }

	for (auto& pCollider : m_vecCollider)
	{
		if (pCollider->GetOwner()->IsActive())
		{
			pCollider->render();
		}
	}
}

void CCamera::SetCameraAsMain()
{
	tEventInfo tEvent = {};
	tEvent.eType      = EVENT_TYPE::SET_CAMEAR_INDEX;
	tEvent.lParam     = (DWORD_PTR)this;
	tEvent.wParam     = 0;

	CEventMgr::GetInst()->AddEvent(tEvent);
}

void CCamera::SetCameraIndex(int _iIdx)
{
	if (m_iCamIdx == _iIdx)
		return;

	tEventInfo tEvent = {};

	tEvent.eType  = EVENT_TYPE::SET_CAMEAR_INDEX;
	tEvent.lParam = (DWORD_PTR)this;
	tEvent.wParam = _iIdx;

	CEventMgr::GetInst()->AddEvent(tEvent);
}

void CCamera::CheckLayerMask(int _iLayerIdx)
{
	if (m_iLayerMask & 1 << _iLayerIdx)
	{
		m_iLayerMask &= ~(1 << _iLayerIdx);
	}
	else
	{
		m_iLayerMask |= 1 << _iLayerIdx;
	}
}

void CCamera::CheckLayerMask(const wstring& _strLayerName)
{
	CScene*       pScene = CSceneMgr::GetInst()->GetCurScene();
	const CLayer* pLayer = pScene->GetLayer(_strLayerName);

	CheckLayerMask(pLayer->GetLayerIdx());
}


void CCamera::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_eProjType, sizeof(UINT), 1, _pFile);

	fwrite(&m_fWidth, sizeof(float), 1, _pFile);
	fwrite(&m_fAspectRatio, sizeof(float), 1, _pFile);
	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_fFar, sizeof(float), 1, _pFile);

	fwrite(&m_iLayerMask, sizeof(UINT), 1, _pFile);
	fwrite(&m_iCamIdx, sizeof(int), 1, _pFile);
}

void CCamera::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_eProjType, sizeof(UINT), 1, _pFile);

	fread(&m_fWidth, sizeof(float), 1, _pFile);
	fread(&m_fAspectRatio, sizeof(float), 1, _pFile);
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_fFar, sizeof(float), 1, _pFile);

	fread(&m_iLayerMask, sizeof(UINT), 1, _pFile);
	fread(&m_iCamIdx, sizeof(int), 1, _pFile);
}
