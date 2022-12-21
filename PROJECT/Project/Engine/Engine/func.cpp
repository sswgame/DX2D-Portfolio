#include "pch.h"

void SaveStringToFile(const string& _str, FILE* _pFile)
{
	const BYTE length = static_cast<BYTE>(_str.length());
	fwrite(&length, sizeof(BYTE), 1, _pFile);
	fwrite(_str.c_str(), sizeof(char), length, _pFile);
}

void LoadStringFromFile(string& _str, FILE* _pFile)
{
	char szBuffer[256]{};
	BYTE length{};
	fread(&length, sizeof(BYTE), 1, _pFile);
	fread(szBuffer, sizeof(char), length, _pFile);

	_str = szBuffer;
}

void SaveWStringToFile(const wstring& _str, FILE* _pFile)
{
	const BYTE length = static_cast<BYTE>(_str.length());
	fwrite(&length, sizeof(BYTE), 1, _pFile);
	fwrite(_str.c_str(), sizeof(wchar_t), length, _pFile);
}

void LoadWStringFromFile(wstring& _str, FILE* _pFile)
{
	wchar_t szBuffer[256]{};
	BYTE    length{};
	fread(&length, sizeof(BYTE), 1, _pFile);
	fread(szBuffer, sizeof(wchar_t), length, _pFile);

	_str = szBuffer;
}


std::wstring ToWString(const std::string& str)
{
	const int    length = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wstr{};
	wstr.resize(length);
	MultiByteToWideChar(CP_ACP, 0, &str[0], -1, &wstr[0], length);
	//널문자 제거
	wstr.pop_back();
	return wstr;
}

std::string ToString(const std::wstring& wstr)
{
	const int   length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str{};
	str.resize(length);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, &str[0], length, nullptr, nullptr);
	//널문자 제거
	str.pop_back();
	return str;
}

std::wstring ToWString(COMPONENT_TYPE _type)
{
	static const wchar_t* szWString[static_cast<UINT>(COMPONENT_TYPE::END)] =
	{
		L"TRANSFORM"
	  , L"CAMERA"
	  , L"COLLIDER2D"
		//, L"COLLIDER3D"
	  , L"ANIMATOR2D"
		//, L"ANIMATOR3D"
	  , L"LIGHT2D"
		//, L"LIGHT3D"
		//, L"BOUNDINGBOX"
	  , L"MESHRENDER"
	  , L"TILEMAP"
	  , L"PARTICLESYSTEM"
		//, L"LANDSCAPE"
		//, L"DECAL"
	   ,
	};

	return szWString[static_cast<UINT>(_type)];
}

std::string ToString(COMPONENT_TYPE _type)
{
	return ToString(ToWString(_type));
}

std::wstring ToWString(RES_TYPE _type)
{
	static const wchar_t* szWString[static_cast<UINT>(RES_TYPE::END)] =
	{
		L"PREFAB"
	  , L"MESHDATA"
	  , L"MATERIAL"
	  , L"GRAPHICS_SHADER"
	  , L"COMPUTE_SHADER"
	  , L"MESH"
	  , L"TEXTURE"
	  , L"SOUND"
	  , L"SCENEFILE"
	  , L"CMETADATA"
	};

	return szWString[static_cast<UINT>(_type)];
}

std::string ToString(RES_TYPE _type)
{
	return ToString(ToWString(_type));
}
