#include "pch.h"
#include "CPathMgr.h"

CPathMgr::CPathMgr()
	:
	m_contentPath{}
  , m_strRelativePath{} {}

CPathMgr::~CPathMgr() {}

void CPathMgr::init()
{
	// Content ���� ��θ� ã�Ƴ���.
	GetCurrentDirectory(256, m_contentPath);

	// �θ� ���� ��θ� �˾Ƴ���( OutputFile ���� )
	int iLen = static_cast<int>(wcslen(m_contentPath));

	for (int i = iLen - 1; 0 <= i; --i)
	{
		if (L'\\' == m_contentPath[i])
		{
			m_contentPath[i] = L'\0';
			break;
		}
	}

	// content ���� ��θ� ���δ�.
	wcscat_s(m_contentPath, L"\\bin\\content\\");
}

const wchar_t* CPathMgr::GetRelativePath(const wstring& _strFilePath)
{
	size_t pos = _strFilePath.find(m_contentPath, 0);
	pos += wcsnlen_s(m_contentPath, 256);

	const std::wstring relativePath = _strFilePath.substr(pos, _strFilePath.length());
	wcscpy_s(m_strRelativePath, relativePath.data());

	return m_strRelativePath;
}
