#include "pch.h"
#include "CTitleScript.h"

#include <Engine/CAnimator2D.h>

#include "CMouseScript.h"
#include "CSceneSaveLoad.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	const wchar_t*        BASIC_MOUSE_PREFAB = L"prefab\\BASIC_MOUSE.pref";
	inline const wchar_t* TILE_BGM           = L"sound\\TITLE.wav";
	//BGM
	inline const wchar_t* DUNGEON_BGM = L"sound\\DUNGEON_FIELD.wav";
}

CTitleScript::CTitleScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::TITLESCRIPT), DEFINE_NAME(CTitleScript)}
  , m_pFrontCloud{nullptr}
  , m_frontCloudSpeed{0}
  , m_maxFrontPosX{0}
  , m_startFrontPosX{0}
  , m_pBackCloud{nullptr}
  , m_backCloudSpeed{0}
  , m_maxBackCloudPosX{0}
  , m_startBacCloudPosX{0}
  , m_selectIndex{0}
  , m_isMouseMove{false}
  , m_menuSelected{}
  , m_arrMenuObjects{}
{
	AddScriptParam("FRONT SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_frontCloudSpeed);
	AddScriptParam("FRONT START X", SCRIPTPARAM_TYPE::FLOAT, &m_startFrontPosX);
	AddScriptParam("FRONT MAX X", SCRIPTPARAM_TYPE::FLOAT, &m_maxFrontPosX);
	AddScriptParam("BACK SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_backCloudSpeed);
	AddScriptParam("BACK START X", SCRIPTPARAM_TYPE::FLOAT, &m_startBacCloudPosX);
	AddScriptParam("BACK MAX X", SCRIPTPARAM_TYPE::FLOAT, &m_maxBackCloudPosX);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_nextSceneName);
}

CTitleScript::~CTitleScript() = default;

CTitleScript::CTitleScript(const CTitleScript& other)
	:
	CScript{other}
  , m_pFrontCloud{nullptr}
  , m_frontCloudSpeed{other.m_frontCloudSpeed}
  , m_maxFrontPosX{other.m_maxFrontPosX}
  , m_startFrontPosX{other.m_startFrontPosX}
  , m_pBackCloud{nullptr}
  , m_backCloudSpeed{other.m_backCloudSpeed}
  , m_maxBackCloudPosX{other.m_maxBackCloudPosX}
  , m_startBacCloudPosX{other.m_startBacCloudPosX}
  , m_selectIndex{0}
  , m_isMouseMove{false}
  , m_menuSelected{}
  , m_arrMenuObjects{}
  , m_pMouseScript{nullptr}
{
	ClearScriptParam();
	AddScriptParam("FRONT SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_frontCloudSpeed);
	AddScriptParam("FRONT START X", SCRIPTPARAM_TYPE::FLOAT, &m_startFrontPosX);
	AddScriptParam("FRONT MAX X", SCRIPTPARAM_TYPE::FLOAT, &m_maxFrontPosX);
	AddScriptParam("BACK SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_backCloudSpeed);
	AddScriptParam("BACK START X", SCRIPTPARAM_TYPE::FLOAT, &m_startBacCloudPosX);
	AddScriptParam("BACK MAX X", SCRIPTPARAM_TYPE::FLOAT, &m_maxBackCloudPosX);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_nextSceneName);
}

void CTitleScript::start()
{
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(DEFAULT_LAYER, L"DEFAULT");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(TILE_LAYER, L"Tile");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(PLAYER_LAYER, L"Player");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(PLAYER_ATTACK_LAYER, L"PlayerAttack");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_LAYER, L"Monster");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_ATTACK_LAYER, L"MonsterAttack");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(MONSTER_SPAWN_LAYER, L"MonsterSpawner");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(ENTER_SCENE_LAYER, L"EnterScene");
	CSceneMgr::GetInst()->GetCurScene()->SetLayerName(UI_LAYER, L"UI");

	CGameObject* pMouse = CSceneMgr::GetInst()->FindObjectByName(L"Mouse");
	m_pMouseScript      = static_cast<CMouseScript*>(pMouse->GetScript(static_cast<UINT>(SCRIPT_TYPE::MOUSESCRIPT)));

	m_pFrontCloud = CSceneMgr::GetInst()->GetCurScene()->FindObject(L"DEFAULT", L"FRONT_CLOUD");
	m_pBackCloud  = CSceneMgr::GetInst()->GetCurScene()->FindObject(L"DEFAULT", L"BACK_CLOUD");
	CDevice::GetInst()->SetClearTargetColor(Vec4{121.f / 255.f, 185.f / 255.f, 1.f, 1.f});
	FindAllChild();

	CResMgr::GetInst()->FindRes<CSound>(TILE_BGM)->Play(10, 1.f);
}

void CTitleScript::update()
{
	MoveCloud();
	UpdateMousePosition();
	CheckInput();
}

void CTitleScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_frontCloudSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_startFrontPosX, sizeof(float), 1, _pFile);
	fwrite(&m_maxFrontPosX, sizeof(float), 1, _pFile);
	fwrite(&m_backCloudSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_startBacCloudPosX, sizeof(float), 1, _pFile);
	fwrite(&m_maxBackCloudPosX, sizeof(float), 1, _pFile);
	SaveWStringToFile(m_nextSceneName, _pFile);
}

void CTitleScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_frontCloudSpeed, sizeof(float), 1, _pFile);
	fread(&m_startFrontPosX, sizeof(float), 1, _pFile);
	fread(&m_maxFrontPosX, sizeof(float), 1, _pFile);
	fread(&m_backCloudSpeed, sizeof(float), 1, _pFile);
	fread(&m_startBacCloudPosX, sizeof(float), 1, _pFile);
	fread(&m_maxBackCloudPosX, sizeof(float), 1, _pFile);
	LoadWStringFromFile(m_nextSceneName, _pFile);
}

void CTitleScript::FindAllChild()
{
	auto& vecChild = GetOwner()->GetChild();
	for (auto& pChild : vecChild)
	{
		if (L"PLAY" == pChild->GetName()) { m_arrMenuObjects[static_cast<UINT>(TITLE_MENU::PLAY)] = pChild; }
		if (L"OPTION" == pChild->GetName()) { m_arrMenuObjects[static_cast<UINT>(TITLE_MENU::OPTION)] = pChild; }
		if (L"EXIT" == pChild->GetName()) { m_arrMenuObjects[static_cast<UINT>(TITLE_MENU::EXIT)] = pChild; }
	}

	for (auto& pGameObject : m_arrMenuObjects)
	{
		pGameObject->Animator2D()->Play(pGameObject->GetName() + L"_OFF", false);
	}
}

void CTitleScript::MoveCloud()
{
	//back cloud
	Vec3 frontCloudPos = m_pFrontCloud->Transform()->GetRelativePos();
	frontCloudPos.x    = std::clamp(frontCloudPos.x += DT * m_frontCloudSpeed, m_startFrontPosX, m_maxBackCloudPosX);
	if (frontCloudPos.x >= m_maxFrontPosX) { frontCloudPos.x = m_startBacCloudPosX; }
	m_pFrontCloud->Transform()->SetRelativePos(frontCloudPos);

	//front cloud
	Vec3 backCloudPos = m_pBackCloud->Transform()->GetRelativePos();
	backCloudPos.x    = std::clamp(backCloudPos.x += DT * m_backCloudSpeed, m_startBacCloudPosX, m_maxBackCloudPosX);
	if (backCloudPos.x >= m_maxBackCloudPosX) { backCloudPos.x = m_startBacCloudPosX; }
	m_pBackCloud->Transform()->SetRelativePos(backCloudPos);
}

void CTitleScript::CheckInput()
{
	for (int i = 0; i < m_arrMenuObjects.size(); ++i)
	{
		if (m_pMouseScript->IsMouseIn(m_arrMenuObjects[i]))
		{
			m_arrMenuObjects[i]->Animator2D()->Play(m_arrMenuObjects[i]->GetName() + L"_ON", false);
			if (m_pMouseScript->IsClicked(m_arrMenuObjects[i], KEY::LBTN))
			{
				DoAction(static_cast<TITLE_MENU>(i));
			}
		}
		else
		{
			m_arrMenuObjects[i]->Animator2D()->Play(m_arrMenuObjects[i]->GetName() + L"_OFF", false);
		}
	}
}

void CTitleScript::DoAction(TITLE_MENU type)
{
	switch (type)
	{
	case TITLE_MENU::PLAY:
		{
			const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
			CScene*            pLoadScene  = CSceneSaveLoad::LoadScene(contentPath + m_nextSceneName);
			pLoadScene->SetAddDonDestroy(true);
			CSceneMgr::GetInst()->GoToNextScene(pLoadScene, true);
			CResMgr::GetInst()->FindRes<CSound>(TILE_BGM)->Stop();
			CResMgr::GetInst()->FindRes<CSound>(DUNGEON_BGM)->Play(50, 1.f);
			CDevice::GetInst()->SetClearTargetColor(Vec4(0.5f, 0.5f, 0.5f, 1.f));
		}
		break;
	case TITLE_MENU::OPTION: break;
	case TITLE_MENU::EXIT:
		PostQuitMessage(0);
		break;
	case TITLE_MENU::END: break;
	default: ;
		break;
	}
}

void CTitleScript::UpdateMousePosition()
{
	static bool test     = false;
	Vec2        mousePos = CKeyMgr::GetInst()->GetMousePos();
	mousePos.x -= CDevice::GetInst()->GetRenderResolution().x * .5f;
	mousePos.y *= -1.f;
	mousePos.y += CDevice::GetInst()->GetRenderResolution().y * .5f;

	m_pMouseScript->Transform()->SetRelativePos(Vec3{mousePos.x, mousePos.y, -5.f});
}
