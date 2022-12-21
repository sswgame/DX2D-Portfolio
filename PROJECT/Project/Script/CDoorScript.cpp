#include "pch.h"
#include "CDoorScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CCollider2D.h>

#include "CStageScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	std::unordered_map<PARTICLE_DIR, Vec2> G_MAP_DIRECTION
		= {
			{PARTICLE_DIR::FROM_LEFT, Vec2{1.f, 0.f}}
		  , {PARTICLE_DIR::FROM_RIGHT, Vec2{-1.f, 0.f}}
		  , {PARTICLE_DIR::FROM_TOP, Vec2{0.f, 1.f}}
		  , {PARTICLE_DIR::FROM_BOTTOM, Vec2{0.f, -1.f}}
		};

	constexpr float ROTATION_EPSILON = 0.001f;
}

CDoorScript::CDoorScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::DOORSCRIPT), DEFINE_NAME(CDoorScript)}
  , m_state{DOOR_STATE::CLOSE}
  , m_direction{PARTICLE_DIR::FROM_BOTTOM}
  , m_pColliderTile{nullptr}
{
	m_keyPrefabEnterScene   = L"prefab\\ENTER_SCENE.pref";
	m_keyPrefabDoorEffect   = L"prefab\\DOOR_EFFECT.pref";
	m_keyPrefabColliderTile = L"prefab\\COLLIDER_TILE.pref";

	AddScriptParam("PLAYER POSITION", SCRIPTPARAM_TYPE::VEC2, &m_playerPos);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_sceneFileName);
}

CDoorScript::~CDoorScript() = default;

CDoorScript::CDoorScript(const CDoorScript& other)
	:
	CScript{other}
  , m_keyPrefabEnterScene{other.m_keyPrefabEnterScene}
  , m_keyPrefabDoorEffect{other.m_keyPrefabDoorEffect}
  , m_keyPrefabColliderTile{other.m_keyPrefabColliderTile}
  , m_playerPos{other.m_playerPos}
  , m_sceneFileName{other.m_sceneFileName}
{
	ClearScriptParam();
	AddScriptParam("PLAYER POSITION", SCRIPTPARAM_TYPE::VEC2, &m_playerPos);
	AddScriptParam("NEXT SCENE", SCRIPTPARAM_TYPE::SCENE, &m_sceneFileName);
}


void CDoorScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_playerPos, sizeof(Vec2), 1, _pFile);
	SaveWStringToFile(m_sceneFileName, _pFile);
}

void CDoorScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_playerPos, sizeof(Vec2), 1, _pFile);
	LoadWStringFromFile(m_sceneFileName, _pFile);
}

void CDoorScript::start()
{
	Animator2D()->Play(L"CLOSE", false);

	m_pColliderTile = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabColliderTile)->Instantiate();
	m_pColliderTile->Transform()->SetRelativePos(Transform()->GetRelativePos());

	const Vec3 rotation = Transform()->GetRelativeRotation();
	if (XMScalarNearEqual(rotation.z, XMConvertToRadians(0.f), ROTATION_EPSILON))
	{
		m_direction = PARTICLE_DIR::FROM_TOP;
		m_pColliderTile->Collider2D()->SetOffsetPos(0.f, -5.f);
		m_pColliderTile->Collider2D()->SetOffsetScale(250.f, 50.f);
	}
	else if (XMScalarNearEqual(rotation.z, XMConvertToRadians(-90.f), ROTATION_EPSILON))
	{
		m_direction = PARTICLE_DIR::FROM_LEFT;
		m_pColliderTile->Collider2D()->SetOffsetPos(-5.f, 0.f);
		m_pColliderTile->Collider2D()->SetOffsetScale(50.f, 250.f);
	}
	else if (XMScalarNearEqual(rotation.z, XMConvertToRadians(90.f), ROTATION_EPSILON))
	{
		m_direction = PARTICLE_DIR::FROM_RIGHT;
		m_pColliderTile->Collider2D()->SetOffsetPos(-5.f, 0.f);
		m_pColliderTile->Collider2D()->SetOffsetScale(50.f, 250.f);
	}
	else if (XMScalarNearEqual(rotation.z, XMConvertToRadians(180.f), ROTATION_EPSILON))
	{
		m_direction = PARTICLE_DIR::FROM_BOTTOM;
		m_pColliderTile->Collider2D()->SetOffsetPos(0.f, -5.f);
		m_pColliderTile->Collider2D()->SetOffsetScale(250.f, 50.f);
	}
	CSceneMgr::GetInst()->SpawnObject(m_pColliderTile, TILE_LAYER);
}

void CDoorScript::lateupdate()
{
	switch (m_state)
	{
	case DOOR_STATE::OPEN:
		{
			if (false == GetOwner()->IsDead()
			    && Animator2D()->GetCurrentAnim()->IsFinish())
			{
				Vec3       position = Transform()->GetRelativePos();
				const Vec3 rotation = Transform()->GetRelativeRotation();

				CGameObject* pEnterScene = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabEnterScene)->Instantiate();
				pEnterScene->Transform()->SetRelativePos(position);
				pEnterScene->Transform()->SetRelativeRotation(rotation);
				CStageScript* pStageScript = static_cast<CStageScript*>(pEnterScene->
					GetScript(static_cast<UINT>(SCRIPT_TYPE::STAGESCRIPT)));
				pStageScript->SetPlayerSpawnPos(m_playerPos);
				pStageScript->SetNextSceneName(m_sceneFileName);
				CSceneMgr::GetInst()->SpawnObject(pEnterScene, ENTER_SCENE_LAYER);

				CGameObject* pDoorEffect = CResMgr::GetInst()->FindRes<CPrefab>(m_keyPrefabDoorEffect)->Instantiate();
				position.y += 40.f;
				position.z = 200.f;
				pDoorEffect->Transform()->SetRelativePos(position);
				CParticleSystem* pParticle = pDoorEffect->ParticleSystem();
				pParticle->SetDirection(G_MAP_DIRECTION[m_direction]);
				CSceneMgr::GetInst()->SpawnObject(pDoorEffect, TILE_LAYER);

				GetOwner()->Destroy();
			}
		}
		break;
	case DOOR_STATE::CLOSE:
		{
			if (Animator2D()->GetCurrentAnim()->IsFinish())
			{
				m_state = DOOR_STATE::IDLE;
				Animator2D()->GetCurrentAnim()->Reset();
				Animator2D()->Play(L"IDLE", true);
			}
		}
		break;
	case DOOR_STATE::IDLE:
		{
			m_accTime += DT;
			if (m_accTime >= 2.f)
			{
				CLayer* pMonsterLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(MONSTER_LAYER);
				CLayer* pSpawnerLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(MONSTER_SPAWN_LAYER);
				if (pMonsterLayer->GetRootObjects().size() + pSpawnerLayer->GetRootObjects().size() == 0)
				{
					m_state = DOOR_STATE::OPEN;
					Animator2D()->GetCurrentAnim()->Reset();
					Animator2D()->Play(L"OPEN", false);
					m_pColliderTile->Destroy();
					m_pColliderTile = nullptr;
				}
				m_accTime = 0.f;
			}
		}
		break;
	default: ;
	}
}
