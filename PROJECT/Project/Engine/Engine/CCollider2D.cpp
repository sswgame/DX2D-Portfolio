#include "pch.h"
#include "CCollider2D.h"

#include "CResMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CTransform.h"
#include "CScript.h"

CCollider2D::CCollider2D()
	:
	CComponent(COMPONENT_TYPE::COLLIDER2D)
  , m_colliderType(COLLIDER2D_TYPE::BOX)
  , m_offsetPos(Vec2(0.f, 0.f))
  , m_offsetScale(Vec2(1.f, 1.f))
  , m_collisionCount(0)
{
	// Collider2D 모양에 맞는 메쉬 참조
	SetCollider2DType(m_colliderType);

	// Collider2D 전용 재질 참조	
	m_pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Collider2DMtrl.mtrl");
}

CCollider2D::CCollider2D(const CCollider2D& origin)
	:
	CComponent(origin)
  , m_colliderType(origin.m_colliderType)
  , m_offsetPos(origin.m_offsetPos)
  , m_offsetScale(origin.m_offsetScale)
  , m_collisionCount(0)
{
	// Collider2D 모양에 맞는 메쉬 참조
	SetCollider2DType(m_colliderType);

	// Collider2D 전용 재질 참조	
	m_pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Collider2DMtrl.mtrl");
}

CCollider2D::~CCollider2D() {}

void CCollider2D::SetCollider2DType(COLLIDER2D_TYPE _type)
{
	m_colliderType = _type;

	if (COLLIDER2D_TYPE::BOX == m_colliderType)
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_LineStrip");
	else
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_LineStrip");
}

void CCollider2D::SetOffsetScale(Vec2 offsetScale)
{
	m_offsetScale = offsetScale;

	if (COLLIDER2D_TYPE::CIRCLE == m_colliderType)
	{
		m_offsetScale.y = m_offsetScale.x;
	}
}

void CCollider2D::SetOffsetScale(float x, float y)
{
	m_offsetScale = Vec2(x, y);

	if (COLLIDER2D_TYPE::CIRCLE == m_colliderType)
	{
		m_offsetScale.y = m_offsetScale.x;
	}
}

void CCollider2D::finalupdate()
{
	const Matrix matTranslation = XMMatrixTranslation(m_offsetPos.x, m_offsetPos.y, 0.f);
	const Matrix matScale       = XMMatrixScaling(m_offsetScale.x, m_offsetScale.y, 1.f);
	m_matColWorld               = matScale * matTranslation;

	const Vec3   scale           = Transform()->GetWorldScale();
	const Matrix matScaleInverse = XMMatrixInverse(nullptr, XMMatrixScaling(scale.x, scale.y, scale.z));

	// 충돌체 상대행렬 * 오브젝트 월드 크기 역행렬(크기^-1) * 오브젝트 월드 행렬(크기 * 회전 * 이동)
	m_matColWorld = m_matColWorld * matScaleInverse * Transform()->GetWorldMat();
}

void CCollider2D::UpdateData()
{
	g_transform.matWorld = m_matColWorld;
	g_transform.matWV    = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP   = g_transform.matWV * g_transform.matProj;

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
	pCB->SetData(&g_transform, sizeof(tTransform));
	pCB->UpdateData();
}

void CCollider2D::render()
{
	UpdateData();

	m_pMaterial->SetScalarParam(SCALAR_PARAM::INT_0, &m_collisionCount);
	m_pMaterial->UpdateData();

	m_pMesh->render();
}

void CCollider2D::OnCollisionEnter(CCollider2D* pOther)
{
	++m_collisionCount;

	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionEnter(pOther->GetOwner());
	}
}

void CCollider2D::OnCollision(CCollider2D* pOther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollision(pOther->GetOwner());
	}
}

void CCollider2D::OnCollisionExit(CCollider2D* pOther)
{
	--m_collisionCount;

	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionExit(pOther->GetOwner());
	}
}

void CCollider2D::SaveToScene(FILE* pFile)
{
	CComponent::SaveToScene(pFile);

	fwrite(&m_colliderType, sizeof(UINT), 1, pFile);
	fwrite(&m_offsetPos, sizeof(Vec2), 1, pFile);
	fwrite(&m_offsetScale, sizeof(Vec2), 1, pFile);
}

void CCollider2D::LoadFromScene(FILE* pFile)
{
	CComponent::LoadFromScene(pFile);

	fread(&m_colliderType, sizeof(UINT), 1, pFile);
	fread(&m_offsetPos, sizeof(Vec2), 1, pFile);
	fread(&m_offsetScale, sizeof(Vec2), 1, pFile);

	SetCollider2DType(m_colliderType);
}
