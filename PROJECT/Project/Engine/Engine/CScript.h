#pragma once

#include "global.h"
#include "CComponent.h"

#include "CTransform.h"
#include "CMeshRender.h"

#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"


enum class SCRIPTPARAM_TYPE
{
	INT,
	FLOAT,
	VEC2,
	VEC4,
	LITERAL,

	TEX,
	PREFAB,
	SCENE,
	SOUND,
};


struct tScriptParamInfo
{
	string           strParamName;
	SCRIPTPARAM_TYPE eType;
	void*            pParam;
};


class CScript :
	public CComponent
{
private:
	const int                m_iScriptID;    // 스크립트 구별용도(스크립트 타입)
	vector<tScriptParamInfo> m_vecParamInfo; // 에디터 노출 변수


public:
	int                             GetScriptType() const { return m_iScriptID; }
	const vector<tScriptParamInfo>& GetScriptParam() { return m_vecParamInfo; }

protected:
	void AddScriptParam(string _strParamName, SCRIPTPARAM_TYPE _eType, void* _pData);
	void ClearScriptParam() { m_vecParamInfo.clear(); }

public:
	void start() override {}
	void update() override {}
	void lateupdate() override {}
	void finalupdate() final {}

	virtual void OnCollisionEnter(CGameObject* _OtherObject) {};
	virtual void OnCollision(CGameObject* _OtherObject) {};
	virtual void OnCollisionExit(CGameObject* _OtherObject) {};

	CScript* Clone() override = 0;


public:
	CScript(int _iScriptType, const std::wstring& scriptName);
	virtual ~CScript();
};
