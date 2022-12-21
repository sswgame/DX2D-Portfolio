#include "pch.h"
#include "CComponent.h"

#include "CAnimation2D.h"
#include "CAnimator2D.h"
#include "CCamera.h"
#include "CCollider2D.h"
#include "CEventMgr.h"
#include "CLight2D.h"
#include "CMeshRender.h"
#include "CParticleSystem.h"
#include "CTileMap.h"
#include "CTransform.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	:
	m_eComType(_eType)
  , m_pOwner{nullptr}
  , m_bActive(true) {}

CComponent::~CComponent() {}

CComponent* CComponent::MakeComponent(const std::wstring& name)
{
	if (L"TRANSFORM" == name) return new CTransform{};
	if (L"CAMERA" == name) return new CCamera{};
	if (L"COLLIDER2D" == name) return new CCollider2D{};
	if (L"ANIMATOR2D" == name) return new CAnimator2D{};
	if (L"LIGHT2D" == name) return new CLight2D{};
	if (L"MESHRENDER" == name) return new CMeshRender{};
	if (L"TILEMAP" == name) return new CTileMap{};
	if (L"PARTICLESYSTEM" == name) return new CParticleSystem{};

	return nullptr;
}

void CComponent::Activate()
{
	tEventInfo info = {};

	info.eType  = EVENT_TYPE::ACTIVATE_COMPONENT;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}

void CComponent::Deactivate()
{
	tEventInfo info = {};

	info.eType  = EVENT_TYPE::DEACTIVATE_COMOPNENT;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}
