#include "pch.h"
#include "CDontDestroyOnLoadScript.h"

#include "NO_SCRIPT/Define.h"

CDontDestroyOnLoadScript::CDontDestroyOnLoadScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::DONTDESTROYONLOADSCRIPT), DEFINE_NAME(CDontDestroyOnLoadScript)} {}

CDontDestroyOnLoadScript::~CDontDestroyOnLoadScript() = default;

void CDontDestroyOnLoadScript::start()
{
	if (SCENE_STATE::PLAY == CSceneMgr::GetInst()->GetCurScene()->GetSceneState())
	{
		CSceneMgr::GetInst()->DontDestroyOnLoad(GetOwner());
	}
}
