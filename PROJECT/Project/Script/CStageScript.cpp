#include "pch.h"
#include "CStageScript.h"

#include <Engine/CMgrScript.h>

#include "CFadeEffectScript.h"
#include "CGravityScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CSceneSaveLoad.h"
#include "CUnitScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	const wchar_t*  FADE_PREFAB_NAME = L"prefab\\FADE_EFFECT.pref";
	constexpr float FADE_TIME        = 1.f;
}
CStageScript::CStageScript()
	:
	CScript{(int)SCRIPT_TYPE::STAGESCRIPT,DEFINE_NAME(CStageScript)}
{
	AddScriptParam("PLAYER POSITION",SCRIPTPARAM_TYPE::VEC2,&m_playerPos);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_sceneFileName);
}

CStageScript::CStageScript(const CStageScript& other)
	:
	CScript{other}
  , m_playerPos{other.m_playerPos}
  , m_sceneFileName{other.m_sceneFileName}
  , m_pFadeScript{nullptr}
{
	ClearScriptParam();
	AddScriptParam("PLAYER POSITION", SCRIPTPARAM_TYPE::VEC2, &m_playerPos);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_sceneFileName);
}

CStageScript::~CStageScript()=default;

void CStageScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_playerPos, sizeof(Vec2), 1, _pFile);
	SaveWStringToFile(m_sceneFileName, _pFile);
}

void CStageScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_playerPos, sizeof(Vec2), 1, _pFile);
	LoadWStringFromFile(m_sceneFileName, _pFile);
}

void CStageScript::update()
{
	if (nullptr != m_pFadeScript && m_pFadeScript->IsFinished())
	{
		m_pFadeScript->Reset();
		m_pFadeScript = nullptr;

		//카메라한테도 효과를 줘야하므로 카메라의 자식으로 이펙트를 발생시킨다.
		CGameObject* pFadeObject = CResMgr::GetInst()->FindRes<CPrefab>(FADE_PREFAB_NAME)->Instantiate();
		CSceneMgr::GetInst()->SpawnObject(pFadeObject, DEFAULT_LAYER);
		CGameObject* pCamera = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera();
		CSceneMgr::GetInst()->AddChild(pCamera, pFadeObject);
		CFadeEffectScript* pScript = static_cast<CFadeEffectScript*>(pFadeObject->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::FADEEFFECTSCRIPT)));
		pScript->SetFadeType(FADE_TYPE::FADE_IN);
		pScript->SetTime(FADE_TIME);

		CGameObject*   pPlayer       = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
		CPlayerScript* pPlayerScript = static_cast<CPlayerScript*>(pPlayer->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT));
		pPlayerScript->SetKeyBoardEnable(true);
		CGravityScript* pGravityScript = static_cast<CGravityScript*>(pPlayer->
			GetScript((UINT)SCRIPT_TYPE::GRAVITYSCRIPT));
		pGravityScript->SetGravity(true);
		CUnitScript* pUnitScript = static_cast<CUnitScript*>(pPlayer->GetScript((UINT)SCRIPT_TYPE::UNITSCRIPT));
		pUnitScript->SetGround(false);


		const Vec3&  prevPlayerPos = pPlayer->Transform()->GetRelativePos();
		pPlayer->Transform()->SetRelativePos(Vec3{m_playerPos.x, m_playerPos.y, prevPlayerPos.z});

		const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
		CScene*            pLoadScene  = CSceneSaveLoad::LoadScene(contentPath + m_sceneFileName);
		CSceneMgr::GetInst()->GoToNextScene(pLoadScene, true);
	}
}

void CStageScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (L"Player" == _OtherObject->GetName())
	{
		CGameObject* pFadeObject = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(FADE_PREFAB_NAME);
		CSceneMgr::GetInst()->SpawnObject(pFadeObject, DEFAULT_LAYER);
		m_pFadeScript = static_cast<CFadeEffectScript*>(pFadeObject->GetScript((UINT)SCRIPT_TYPE::FADEEFFECTSCRIPT));
		m_pFadeScript->SetFadeType(FADE_TYPE::FADE_OUT);
		m_pFadeScript->SetTime(FADE_TIME);

		CPlayerScript* pPlayerScript = static_cast<CPlayerScript*>(_OtherObject->
			GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT));
		pPlayerScript->SetKeyBoardEnable(false);
	}
}
