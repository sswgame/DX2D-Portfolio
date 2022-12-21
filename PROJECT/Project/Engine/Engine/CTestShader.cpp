#include "pch.h"
#include "CTestShader.h"

CTestShader::CTestShader()
	:
	CComputeShader(32, 32, 1) {}

CTestShader::~CTestShader() {}

void CTestShader::UpdateData()
{
	m_pOutputTex->UpdateData_CS(0, false);

	// 호출 그룹 개수 계산
	m_groupPerThreadCountX;
	m_iGroupPerThreadCountY;

	m_groupCountX = static_cast<UINT>(m_pOutputTex->Width()) / m_groupPerThreadCountX + !!(
		                static_cast<UINT>(m_pOutputTex->Width()) % m_groupPerThreadCountX);
	m_groupCountY = static_cast<UINT>(m_pOutputTex->Height()) / m_iGroupPerThreadCountY + !!(
		                static_cast<UINT>(m_pOutputTex->Height()) % m_iGroupPerThreadCountY);
	m_groupCountZ = 1;
}

void CTestShader::Clear()
{
	CTexture::ClearCS(0);
}
