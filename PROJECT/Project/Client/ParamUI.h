#pragma once

class CTexture;

#include "ListUI.h"

class ParamUI
{
public:
	static int KeyCount;

public:
	static int   Param_Int(const string& _strName, const int* _pInOut);
	static float Param_Float(const string& _strName, const float* _pInOut);
	static Vec2  Param_Vec2(const string& _strName, const Vec2* _pInOut);
	static Vec4  Param_Vec4(const string& _strName, const Vec4* _pInOut);

	static void Param_Literal(const std::string& name, std::wstring* pInOut);

	// 반환값 : 리스트UI 활성화 했을 때 true 아니면 false
	static bool Param_Tex(const string&                    _strName
	                    , CTexture*                        _pCurTex
	                    , std::function<void(void* pData)> _pFunc = nullptr);

	static bool Param_Prefab(const std::string&  name
	                       , const std::wstring& prefabName
	                       , std::function<void(void* pData)> pFunc = nullptr);
	static bool Param_Scene(const std::string&  name
	                       , const std::wstring& prefabName
	                       , std::function<void(void* pData)> pFunc = nullptr);
	static bool Param_Sound(const std::string&               name
	                      , const std::wstring&              soundName
	                      , std::function<void(void* pData)> pFunc = nullptr);

};
