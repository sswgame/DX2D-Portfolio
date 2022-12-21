#pragma once
#include "CShader.h"

#include "CDevice.h"

#include "CTexture.h"
#include "CStructuredBuffer.h"


class CComputeShader
	: public CShader
{
private:
	ComPtr<ID3DBlob>            m_CSBlob;
	ComPtr<ID3D11ComputeShader> m_CS;

protected:
	UINT m_groupCountX;
	UINT m_groupCountY;
	UINT m_groupCountZ;

	UINT m_groupPerThreadCountX;
	UINT m_iGroupPerThreadCountY;
	UINT m_iGroupPerThreadCountZ;

	tScalarParam m_Param;


public:
	void Excute();
	void Excute(UINT _GroupX, UINT _GroupY, UINT _GroupZ);


protected:
	void         UpdateData() override = 0;
	virtual void Clear() = 0;

public:
	int CreateComputeShader(const wstring& _strRelativePath, const string& _strFunc);

public:
	CComputeShader();
	CComputeShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
	virtual ~CComputeShader();
};
