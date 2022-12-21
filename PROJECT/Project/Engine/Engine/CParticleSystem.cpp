#include "pch.h"
#include "CParticleSystem.h"

#include "CTimeMgr.h"

#include "CTransform.h"
#include "CResMgr.h"

CParticleSystem::CParticleSystem()
	:
	CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
  , m_maxCount(1000)
  , m_isPosInherit(0)
  , m_aliveCount(4)
  , m_minLifeTime(0.5f)
  , m_maxLifeTime(2.f)
  , m_startSpeed(100.f)
  , m_endSpeed(10.f)
  , m_startColor(Vec4(1.f, 0.2f, 0.7f, 1.f))
  , m_endColor(Vec4(1.f, 1.f, 1.f, 1.f))
  , m_startScale(Vec3(10.f, 10.f, 1.f))
  , m_endScale(Vec3(1.f, 1.f, 1.f))
  , m_creationRange(50.f)
  , m_creationTerm(0.02f)
  , m_accTime(0.f)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\ParticleRenderMtrl.mtrl"));

	/*m_pCS = static_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").
	                                                                Get());*/

	m_pParticleBuffer = new CStructuredBuffer{};
	m_pParticleBuffer->Create(sizeof(tParticle), m_maxCount, SB_TYPE::READ_WRITE, false, nullptr);

	m_pDataBuffer = new CStructuredBuffer{};
	m_pDataBuffer->Create(sizeof(tParticleData), 1, SB_TYPE::READ_WRITE, true, nullptr);
}

CParticleSystem::CParticleSystem(const CParticleSystem& _origin)
	:
	CRenderComponent(_origin)
  , m_maxCount(_origin.m_maxCount)
  , m_isPosInherit(_origin.m_isPosInherit)
  , m_aliveCount(_origin.m_aliveCount)
  , m_minLifeTime(_origin.m_minLifeTime)
  , m_maxLifeTime(_origin.m_maxLifeTime)
  , m_startSpeed(_origin.m_startSpeed)
  , m_endSpeed(_origin.m_endSpeed)
  , m_startColor(_origin.m_startColor)
  , m_endColor(_origin.m_endColor)
  , m_startScale(_origin.m_startScale)
  , m_endScale(_origin.m_endScale)
  , m_creationRange(_origin.m_creationRange)
  , m_creationTerm(_origin.m_creationTerm)
  , m_accTime(_origin.m_accTime)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\ParticleRenderMtrl.mtrl"));

	/*m_pCS = static_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").
	                                                                Get());*/

	m_pCS = _origin.m_pCS;

	m_pParticleBuffer = new CStructuredBuffer{};
	m_pParticleBuffer->Create(sizeof(tParticle), m_maxCount, SB_TYPE::READ_WRITE, false, nullptr);

	m_pDataBuffer = new CStructuredBuffer{};
	m_pDataBuffer->Create(sizeof(tParticleData), 1, SB_TYPE::READ_WRITE, true, nullptr);
}

CParticleSystem::~CParticleSystem()
{
	SAFE_DELETE(m_pParticleBuffer);
	SAFE_DELETE(m_pDataBuffer);
}


void CParticleSystem::finalupdate()
{
	if (nullptr == m_pCS) { return; }

	m_accTime += DT;
	if (m_creationTerm < m_accTime)
	{
		m_accTime = 0.f;

		tParticleData data{};
		data.aliveCount = m_aliveCount;
		m_pDataBuffer->SetData(&data, 1);
	}

	m_pCS->SetParticleCreateDistance(m_creationRange);
	m_pCS->SetParticleBuffer(m_pParticleBuffer);
	m_pCS->SetParticleDataBuffer(m_pDataBuffer);

	m_pCS->SetParticleMinMaxTime(m_minLifeTime, m_maxLifeTime);
	m_pCS->SetStartEndSpeed(m_startSpeed, m_endSpeed);
	m_pCS->SetStartEndColor(m_startColor, m_endColor);
	m_pCS->SetStartEndScale(m_startScale, m_endScale);
	m_pCS->SetDirection(m_direction);
	m_pCS->SetObjectWorldPos(Transform()->GetWorldPos());

	m_pCS->Excute();
}

void CParticleSystem::render()
{
	if (nullptr == m_pCS) { return; }

	Transform()->UpdateData();

	m_pParticleBuffer->UpdateData(GS | PS, 16);

	GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_1, &m_isPosInherit);
	GetMaterial()->UpdateData();
	GetMesh()->render_particle(m_maxCount);

	m_pParticleBuffer->Clear();
}

void CParticleSystem::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);

	std::wstring key{};
	if (nullptr != m_pCS)
	{
		key = m_pCS->GetKey();
	}
	SaveWStringToFile(key, _pFile);

	fwrite(&m_maxCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_isPosInherit, sizeof(int), 1, _pFile);
	fwrite(&m_aliveCount, sizeof(int), 1, _pFile);
	fwrite(&m_minLifeTime, sizeof(float), 1, _pFile);
	fwrite(&m_maxLifeTime, sizeof(float), 1, _pFile);
	fwrite(&m_startSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_endSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_startColor, sizeof(Vec4), 1, _pFile);
	fwrite(&m_endColor, sizeof(Vec4), 1, _pFile);
	fwrite(&m_startScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_endScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_creationRange, sizeof(float), 1, _pFile);
	fwrite(&m_creationTerm, sizeof(float), 1, _pFile);
	fwrite(&m_direction, sizeof(Vec2), 1, _pFile);
}

void CParticleSystem::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);

	std::wstring key{};
	LoadWStringFromFile(key, _pFile);
	m_pCS = static_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(key).Get());

	UINT maxCount{};
	fread(&maxCount, sizeof(UINT), 1, _pFile);
	SetMaxParticleCount(maxCount);

	fread(&m_isPosInherit, sizeof(int), 1, _pFile);
	fread(&m_aliveCount, sizeof(int), 1, _pFile);
	fread(&m_minLifeTime, sizeof(float), 1, _pFile);
	fread(&m_maxLifeTime, sizeof(float), 1, _pFile);
	fread(&m_startSpeed, sizeof(float), 1, _pFile);
	fread(&m_endSpeed, sizeof(float), 1, _pFile);
	fread(&m_startColor, sizeof(Vec4), 1, _pFile);
	fread(&m_endColor, sizeof(Vec4), 1, _pFile);
	fread(&m_startScale, sizeof(Vec3), 1, _pFile);
	fread(&m_endScale, sizeof(Vec3), 1, _pFile);
	fread(&m_creationRange, sizeof(float), 1, _pFile);
	fread(&m_creationTerm, sizeof(float), 1, _pFile);
	fread(&m_direction, sizeof(Vec2), 1, _pFile);
}


void CParticleSystem::SetMaxParticleCount(UINT count)
{
	if (m_maxCount < count)
	{
		m_pParticleBuffer->Create(sizeof(tParticle), count, SB_TYPE::READ_WRITE, false, nullptr);
	}
	m_maxCount = count;
}

void CParticleSystem::SetPosInherit(bool enable) { m_isPosInherit = enable; }

void CParticleSystem::SetAliveCount(UINT count)
{
	m_aliveCount = std::clamp(count, 0u, m_maxCount);
}

void CParticleSystem::SetMinMaxLifeTime(float min, float max)
{
	m_minLifeTime = std::clamp(min, 0.f, std::numeric_limits<float>::max());
	m_maxLifeTime = std::clamp(max, 0.f, std::numeric_limits<float>::max());
}

void CParticleSystem::SetMinMaxSpeed(float min, float max)
{
	m_startSpeed = min;
	m_endSpeed   = max;
}

void CParticleSystem::SetStartEndColor(Vec4 startColor, Vec4 endColor)
{
	m_startColor = startColor;
	m_endColor   = endColor;
}

void CParticleSystem::SetStartEndScale(Vec3 startScale, Vec3 endScale)
{
	m_startScale = startScale;
	m_endScale   = endScale;
}

void CParticleSystem::SetRange(float range)
{
	m_creationRange = std::clamp(range, 0.f, std::numeric_limits<float>::max());
}

void CParticleSystem::SetTerm(float term)
{
	m_creationTerm = std::clamp(term, 0.f, std::numeric_limits<float>::max());
}
