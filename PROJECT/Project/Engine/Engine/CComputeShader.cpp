#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CPathMgr.h"


CComputeShader::CComputeShader()
	:
	CShader(RES_TYPE::COMPUTE_SHADER)
  , m_groupCountX(0)
  , m_groupCountY(0)
  , m_groupCountZ(0)
  , m_groupPerThreadCountX(0)
  , m_iGroupPerThreadCountY(0)
  , m_iGroupPerThreadCountZ(0)
  , m_Param{} {}

CComputeShader::CComputeShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	:
	CShader(RES_TYPE::COMPUTE_SHADER)
  , m_groupCountX(0)
  , m_groupCountY(0)
  , m_groupCountZ(0)
  , m_groupPerThreadCountX(_iGroupPerThreadX)
  , m_iGroupPerThreadCountY(_iGroupPerThreadY)
  , m_iGroupPerThreadCountZ(_iGroupPerThreadZ)
  , m_Param{} {}

CComputeShader::~CComputeShader() {}

void CComputeShader::Excute()
{
	// 리소스 바인딩
	UpdateData();

	// 상수 업데이트
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SCALAR_PARAM);
	pCB->SetData(&m_Param, sizeof(tScalarParam));
	pCB->UpdateData_CS();

	// 사용할 컴퓨트 쉐이더 세팅
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);

	// 컴퓨트 쉐이더 실행(그룹 개수 지정)
	CONTEXT->Dispatch(m_groupCountX, m_groupCountY, m_groupCountZ);

	// 리소스 해제
	Clear();
}

void CComputeShader::Excute(UINT _GroupX, UINT _GroupY, UINT _GroupZ)
{
	UpdateData();

	// 상수 업데이트
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SCALAR_PARAM);
	pCB->SetData(&m_Param, sizeof(tScalarParam));
	pCB->UpdateData_CS();

	// 사용할 컴퓨트 쉐이더 세팅
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);

	// 컴퓨트 쉐이더 실행(그룹 개수 지정)	
	CONTEXT->Dispatch(_GroupX, _GroupY, _GroupZ);

	// 리소스 해제
	Clear();
}

int CComputeShader::CreateComputeShader(const wstring& _strRelativePath, const string& _strFunc)
{
#ifdef _DEBUG
	static UINT g_iFlag = D3DCOMPILE_DEBUG;
#else
static UINT g_iFlag = 0;
#endif

	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

	// 컴퓨트 쉐이더(HLSL) 컴파일
	HRESULT hr = D3DCompileFromFile(wstring(strContentPath + _strRelativePath).c_str()
	                              , nullptr
	                              , D3D_COMPILE_STANDARD_FILE_INCLUDE
	                              , _strFunc.c_str()
	                              , "cs_5_0"
	                              , g_iFlag
	                              , 0
	                              , m_CSBlob.GetAddressOf()
	                              , m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr
		          , static_cast<char*>(m_ErrBlob->GetBufferPointer())
		          , "Compute Shader Compile Failed!!"
		          , MB_OK);
		assert(nullptr);
		return E_FAIL;
	}

	// 컴파일 된 코드로 ComputeShader 객체 만들기	
	if (FAILED(DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer(), m_CSBlob->GetBufferSize()
		         , nullptr, m_CS.GetAddressOf())))
	{
		assert(nullptr);
		return E_FAIL;
	}

	return S_OK;
}
