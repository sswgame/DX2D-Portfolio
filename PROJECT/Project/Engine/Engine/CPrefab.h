#pragma once
#include "CRes.h"

class CGameObject;

class CPrefab final
	: public CRes
{
private:
	CGameObject* m_pProtoObj;

public:
	using SaveFunc = void(*)(CPrefab*, const wstring&);
	using LoadFunc = int(*)(CPrefab*, const wstring&);
	static SaveFunc m_pSaveFunc;
	static LoadFunc m_pLoadFunc;

public:
	CGameObject* Instantiate();
	CGameObject* GetProto() const { return m_pProtoObj; }
	void         SetProto(CGameObject* _pProto) { m_pProtoObj = _pProto; }

public:
	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

public:
	CPrefab();
	CPrefab(CGameObject* _pProtoObj);
	virtual ~CPrefab();
};
