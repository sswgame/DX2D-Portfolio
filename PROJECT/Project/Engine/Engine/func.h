#pragma once
#include <random>


template <typename T>
void Safe_Del_Vec(vector<T*>& vecData)
{
	for (size_t i = 0; i < vecData.size(); ++i)
	{
		SAFE_DELETE(vecData[i]);
	}
	vecData.clear();
}

template <typename T>
void Safe_Del_list(list<T*>& listData)
{
	typename list<T*>::iterator iter = listData.begin();

	for (; iter != listData.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	listData.clear();
}

template <typename key, typename value>
void Safe_Del_Map(map<key, value>& mapData)
{
	typename map<key, value>::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	mapData.clear();
}

template <typename T1, typename T2>
void Safe_Del_Map(std::unordered_map<T1, T2>& mapData)
{
	for (auto& pair : mapData)
	{
		SAFE_DELETE(pair.second);
	}
	mapData.clear();
}

template <typename T, UINT size>
void Safe_Del_Arr(T(&_Arr)[size])
{
	for (UINT i = 0; i < size; ++i)
	{
		SAFE_DELETE(_Arr[i]);
	}
}


void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);


std::wstring ToWString(COMPONENT_TYPE _type);
std::string  ToString(COMPONENT_TYPE _type);
std::wstring ToWString(RES_TYPE _type);
std::string  ToString(RES_TYPE _type);

std::wstring ToWString(const std::string& str);
std::string  ToString(const std::wstring& wstr);

template <typename T>
T GetRandom()
{
	static std::random_device device{};
	static std::mt19937_64    generator{ device() };

	T value{};
	if (std::is_integral_v<T>)
	{
		const std::uniform_int<int> randomValue{};
		value = static_cast<T>(randomValue(generator));
	}
	else if (std::is_floating_point_v<T>)
	{
		const std::uniform_real<float> randomValue{};
		value = static_cast<T>(randomValue(generator));
	}
	return value;
}

template <typename T>
T GetRandomInRange(T from, T to)
{
	static std::random_device device{};
	static std::mt19937_64    generator{ device() };

	T value{};
	if (std::is_integral_v<T>)
	{
		int                                start = static_cast<int>(from);
		int                                end = static_cast<int>(to);
		std::uniform_int_distribution<int> dist{ start, end };

		value = static_cast<T>(dist(generator));
	}
	else if (std::is_floating_point_v<T>)
	{
		float                                 start = static_cast<float>(from);
		float                                 end = static_cast<float>(to);
		std::uniform_real_distribution<float> dist{ start, end };

		value = static_cast<T>(dist(generator));
	}
	return value;
}

#include "Ptr.h"
#include "CResMgr.h"

template <typename RES>
void SaveResPtr(Ptr<RES> _ptr, FILE* _pFile)
{
	bool bNullPtr = nullptr == _ptr ? true : false;
	fwrite(&bNullPtr, sizeof(bool), 1, _pFile);

	if (!bNullPtr)
	{
		SaveWStringToFile(_ptr->GetKey(), _pFile);
		SaveWStringToFile(_ptr->GetRelativePath(), _pFile);
	}
}

template <typename RES>
void LoadResPtr(Ptr<RES>& _ptr, FILE* _pFile)
{
	bool bNull = false;
	fread(&bNull, sizeof(bool), 1, _pFile);

	if (!bNull)
	{
		wstring strKey, strRelativePath;

		LoadWStringFromFile(strKey, _pFile);
		LoadWStringFromFile(strRelativePath, _pFile);

		_ptr = CResMgr::GetInst()->Load<RES>(strKey, strRelativePath);
	}
}
