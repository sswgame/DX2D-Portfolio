#include "pch.h"
#include "CParticleUpdateShader.h"


namespace
{
	constexpr int  DEFAULT_GROUP_COUNT_X                = 32;
	constexpr int  DEFAULT_GROUP_COUNT_Y                = 1;
	constexpr int  DEFAULT_GROUP_COUNT_Z                = 1;
	constexpr UINT DEFAULT_UAV_REGISTER_NUMBER_PARTICLE = 0;
	constexpr UINT DEFAULT_UAV_REGISTER_NUMBER_DATA     = 1;
}
CParticleUpdateShader::CParticleUpdateShader()
	:
	CComputeShader(DEFAULT_GROUP_COUNT_X, DEFAULT_GROUP_COUNT_Y, DEFAULT_GROUP_COUNT_Z)
  , m_pParticleBuffer(nullptr)
  , m_pDataBuffer{nullptr} {}

CParticleUpdateShader::~CParticleUpdateShader() = default;

void CParticleUpdateShader::UpdateData()
{
	m_pParticleBuffer->UpdateData_CS(DEFAULT_UAV_REGISTER_NUMBER_PARTICLE, false);
	m_pDataBuffer->UpdateData_CS(DEFAULT_UAV_REGISTER_NUMBER_DATA, false);



	m_groupCountX = m_pParticleBuffer->GetElementCount() / m_groupPerThreadCountX
	                + !!(m_pParticleBuffer->GetElementCount() % m_groupPerThreadCountX);
	m_groupCountY = 1;
	m_groupCountZ = 1;

	// Max Thread Count
	m_Param.iArr[0] = m_groupCountX * m_groupPerThreadCountX;
	m_Param.iArr[1] = m_pParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::Clear()
{
	m_pParticleBuffer->Clear();
}
