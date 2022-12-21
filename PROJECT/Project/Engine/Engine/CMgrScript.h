#pragma once
#include "CScript.h"

#include "CSceneMgr.h"
#include "CScene.h"

#undef max

class CSingletonScript
	: public CScript
{
public:
	CSingletonScript()
		:
		CScript{std::numeric_limits<int>::max(), L"DummyObject"} {}

	virtual ~CSingletonScript() { s_pInstance = nullptr; }
	CLONE(CSingletonScript);

protected:
	inline static CGameObject* s_pInstance = nullptr;
};
template <typename T>
class CMgrScript :
	public CSingletonScript
{
public:
	static T* GetInst()
	{
		if (nullptr == s_pInstance)
		{
			s_pInstance = new CGameObject{};
			s_pInstance->SetName(L"DummyObject");
			CSceneMgr::GetInst()->GetCurScene()->AddObject(s_pInstance, 0);
			CSceneMgr::GetInst()->DontDestroyOnLoad(s_pInstance);
		}

		T* pScript = s_pInstance->GetScript<T>();
		if (nullptr == pScript)
		{
			pScript = new T{};
			s_pInstance->AddComponent(pScript);
			pScript->start();

			return pScript;
		}
		else
		{
			return pScript;
		}
		return nullptr;
	}

public:
	CMgrScript()          = default;
	virtual ~CMgrScript() = default;

private:
};
