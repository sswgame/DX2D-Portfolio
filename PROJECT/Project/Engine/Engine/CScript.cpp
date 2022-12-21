#include "pch.h"
#include "CScript.h"

void CScript::AddScriptParam(string _strParamName, SCRIPTPARAM_TYPE _eType, void* _pData)
{
	m_vecParamInfo.push_back(tScriptParamInfo{_strParamName, _eType, _pData});
}

CScript::CScript(int _iScriptType, const std::wstring& name)
	:
	CComponent(COMPONENT_TYPE::SCRIPT)
  , m_iScriptID(_iScriptType)
{
	SetName(name);
}

CScript::~CScript() {}
