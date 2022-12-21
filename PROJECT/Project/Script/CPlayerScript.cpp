#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/CAnimator2D.h>

#include <array>
#include <Engine/CMgrScript.h>

#include "CAfterImageScript.h"
#include "CInventoryUIScript.h"
#include "CMouseScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerInfoUIScript.h"
#include "CUIManagerScript.h"
#include "CUnitScript.h"
#include "CWeaponScript.h"

#undef min
#undef max
namespace
{
	std::array<const wchar_t*, 4> g_suffix = { L"idle", L"run", L"jump", L"die" };

	std::array<const wchar_t*, static_cast<UINT>(COSTUME_TYPE::END)> g_prefix = {
		L"basic_"
	  , L"alice_"
	  , L"pikax_"
	  , L"metalPlate_"
	  , L"redLotus_"
	};
	constexpr float JUMP_POWER     = 6000.f;
	constexpr float DASH_SPEED     = 1500.f;
	constexpr float PLAYER_SPEED   = 400.f;
	constexpr int   DASH_COUNT     = 2;
	constexpr float DASH_COOLTIME  = 1.5f;
	constexpr float DASH_TIME      = 0.19f;
	constexpr float ATTACKED_DELAY = 0.2f;

	inline const wchar_t* PLAYER_DEAD = L"sound\\PLAYER_DEAD.wav";
	inline const wchar_t* PLAYER_JUMP = L"sound\\JUMP.wav";
	inline const wchar_t* PLAYER_DASH = L"sound\\DASH.wav";
	inline const wchar_t* FAIRY = L"sound\\FAIRY.wav";
	inline const wchar_t* GOLD = L"sound\\GOLD.wav";
	inline const wchar_t* HIT_PLAYER = L"sound\\HIT_PLAYER.wav";
	inline const wchar_t* WEAPON_SWAP = L"sound\\WEAPON_SWAP.wav";
	inline const wchar_t* OPEN_INVENTORY = L"sound\\OPEN_INVENTORY.wav";
	inline const wchar_t* COLLECT_ITEM = L"sound\\COLLECT_ITEM.wav";
	//PLAYER
	inline const wchar_t* PLAYER_WALK[] = {
		L"sound\\PLAYER_WALK_1.wav"
	  , L"sound\\PLAYER_WALK_2.wav"
	  , L"sound\\PLAYER_WALK_3.wav"
	  , L"sound\\PLAYER_WALK_4.wav"
	   ,
	};
}

CPlayerScript::CPlayerScript()
	:
	CScript{ static_cast<int>(SCRIPT_TYPE::PLAYERSCRIPT), DEFINE_NAME(CPlayerScript) }
	, m_pUnitScript{ nullptr }
	, m_pMouseScript{ nullptr }
	, m_pPlayerInfoUIScript{ nullptr }
	, m_pInventoryUIScript{ nullptr }
	, m_pWeaponHand{ nullptr }
	, m_pItemHolderScript{ nullptr }
	, m_isHalfTilePass{ false }
	, m_accTime{ 0 }
	, m_isCostumeChanged{ false }
	, m_maxDashCount{ 0 }
	, m_dashCount{ DASH_COUNT }
	, m_dashCoolTime{ 0 }
	, m_remainDashTime{ 0 }
	, m_isDash{ false }
	, m_showDustEffect{ false }
	, m_isJump{ false }
	, m_isAttacked{ 0 }
	, m_speed{ 0 }
	, m_costumeType{ COSTUME_TYPE::END }
	, m_pDustEffect{ nullptr }
	, m_keyBoardEnable{ true }
{
	m_maxDashCount = m_dashCount;
	AddScriptParam("PlayerSpeed", SCRIPTPARAM_TYPE::FLOAT, &m_speed);

	m_keyPrefabWarning = L"prefab\\WARNING.pref";
	m_keyPrefabAfterImage = L"prefab\\AfterImage.pref";
}

CPlayerScript::~CPlayerScript() = default;

void CPlayerScript::start()
{
	m_pUnitScript = static_cast<CUnitScript*>(GetOwner()->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
	m_pUnitScript->SetHP(static_cast<int>(UNIT_HP::PLAYRER));
	m_pUnitScript->SetMaxHP(m_pUnitScript->GetHP());
	CGameObject* pMouse = CMgrScript<CObjectManagerScript>::GetInst()->GetMouse();
	m_pMouseScript = static_cast<CMouseScript*>(pMouse->GetScript(static_cast<UINT>(SCRIPT_TYPE::MOUSESCRIPT)));

	CGameObject* pInventory = CMgrScript<CUIManagerScript>::GetInst()->GetUI(UI_TYPE::INVENTORY);
	m_pInventoryUIScript
		= static_cast<CInventoryUIScript*>(pInventory->GetScript(static_cast<UINT>(SCRIPT_TYPE::INVENTORYUISCRIPT)));

	CGameObject* pPlayerInfo = CMgrScript<CUIManagerScript>::GetInst()->GetUI(UI_TYPE::PLAYER_INFO);
	m_pPlayerInfoUIScript
		= static_cast<CPlayerInfoUIScript*>(pPlayerInfo->GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERINFOUISCRIPT)));

	FindAllChild();

	ChangeCostume(COSTUME_TYPE::BASIC);
}

void CPlayerScript::update()
{
	// 상태 설정
	SetState();
	// 방향 설정
	SetDirection();
	//입력에 따른 플레이어 위치 설정
	if (m_keyBoardEnable)
	{
		UpdatePosition();
		//입력에 따른 플레이어 행동 설정
		UpdateAction();
	}

	UpdateData();
}

void CPlayerScript::UpdateData()
{
	bool isAttacked = m_isAttacked;
	if (true == isAttacked)
	{
		m_accTime += DT;
		if (m_accTime > ATTACKED_DELAY)
		{
			m_isAttacked = false;
			m_accTime = 0.f;
		}
	}
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, &m_isAttacked);
}

void CPlayerScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetLayerIndex() == MONSTER_ATTACK_LAYER)
	{
		//이전에 공격을 당한 상태면 잠깐 동안 무적으로 피를 잃지 않아야 한다.
		if (false == m_isAttacked)
		{
			OnAttacked();
		}
	}
}

void CPlayerScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_speed, sizeof(float), 1, _pFile);
	float test{ 1 };
	fwrite(&test, sizeof(float), 1, _pFile);
}

void CPlayerScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_speed, sizeof(float), 1, _pFile);
	float test{};
	fread(&test, sizeof(float), 1, _pFile);
}

void CPlayerScript::Jump()
{
	m_pUnitScript->GetGravityScript()->SetVelocity(Vec2{ 0.f, JUMP_POWER });
	m_pUnitScript->SetGround(false);
	m_isJump = true;

	CResMgr::GetInst()->Load<CSound>(PLAYER_JUMP, PLAYER_JUMP)->Play(1, 1.f);
}

void CPlayerScript::Dash()
{
	const Vec3 position = Transform()->GetRelativePos();
	const Vec3 mousePosition = m_pMouseScript->Transform()->GetRelativePos();

	Vec2 forceDirection = Vec2{ mousePosition.x - position.x, mousePosition.y - position.y };
	forceDirection.Normalize();

	const Vec2 velocity = forceDirection * DASH_SPEED;
	m_pUnitScript->GetGravityScript()->SetVelocity(velocity);
	m_pUnitScript->SetGround(false);

	m_dashCount = std::max(0, --m_dashCount);
	m_pPlayerInfoUIScript->SetDashCount(m_dashCount);
	m_isHalfTilePass = true;
	m_isDash = true;

	CResMgr::GetInst()->Load<CSound>(PLAYER_DASH, PLAYER_DASH)->Play(1, 1.f);
}

void CPlayerScript::OnAttacked()
{
	m_isAttacked = true;
	CResMgr::GetInst()->Load<CSound>(HIT_PLAYER, HIT_PLAYER)->Play(1, 1.f);
	//hp update
	int hp = m_pUnitScript->GetHP();
	hp -= 10;
	m_pUnitScript->SetHP(hp);
	if (hp < 0)
	{
		//플레이 도중 죽지 않기 위해서(방송용)
		m_pUnitScript->SetHP(m_pUnitScript->GetMaxHP());
		m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
	}
	m_pPlayerInfoUIScript->SetHP(static_cast<float>(hp) / m_pUnitScript->GetMaxHP());

	//shake camera
	static CGameObject*   pCamera       = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera();
	static CCameraScript* pCameraScript = static_cast<CCameraScript*>(pCamera->
		GetScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT)));
	pCameraScript->ShakeCamera(0.2f);

	//warn
	CGameObject* pWarn = CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabWarning);
	pWarn->Transform()->SetRelativePos(pCamera->Transform()->GetRelativePos());
	CSceneMgr::GetInst()->SpawnObject(pWarn, DEFAULT_LAYER);
}

void CPlayerScript::UpdatePosition()
{
	const Vec3 position = Transform()->GetRelativePos();
	Vec3       moveAmount = {};
	m_showDustEffect = false;

	if (KEY_PRESSED(KEY::D))
	{
		m_showDustEffect = true;
		moveAmount.x = PLAYER_SPEED * DT;

		CResMgr::GetInst()->Load<CSound>(PLAYER_WALK[3], PLAYER_WALK[3])->Play(1, 1.f);
	}
	else if (KEY_PRESSED(KEY::A))
	{
		m_showDustEffect = true;
		moveAmount.x = -PLAYER_SPEED * DT;
		CResMgr::GetInst()->Load<CSound>(PLAYER_WALK[3], PLAYER_WALK[3])->Play(1, 1.f);
	}

	ShowDustEffect();

	if (KEY_PRESSED(KEY::S) && KEY_TAP(KEY::SPACE))
	{
		m_isHalfTilePass = true;
		m_pUnitScript->SetGround(false);
	}

	// 중력 스크립트 확인
	if (false == m_pUnitScript->HasGravityScript())
	{
		Transform()->SetRelativePos(position + moveAmount);
		return;
	}

	if ((false == (KEY_PRESSED(KEY::S)) && KEY_TAP(KEY::SPACE) || KEY_TAP(KEY::W)) && m_pUnitScript->IsGround())
	{
		Jump();
		m_pUnitScript->SetUnitState(UNIT_STATE::JUMP);
	}
	else
	{
		m_isJump = false;
	}

	if (KEY_TAP(KEY::RBTN)
		&& false == CMgrScript<CUIManagerScript>::GetInst()->GetUI(UI_TYPE::INVENTORY)->IsActive())
	{
		if (0 != m_dashCount)
		{
			Dash();
			m_pUnitScript->SetUnitState(UNIT_STATE::JUMP);
		}
	}

	if (m_dashCount != DASH_COUNT)
	{
		m_dashCoolTime += DT;
		if (DASH_COOLTIME <= m_dashCoolTime)
		{
			m_dashCoolTime = 0.f;
			m_dashCount = std::min(++m_dashCount, m_maxDashCount);
			m_pPlayerInfoUIScript->SetDashCount(m_dashCount);
		}
	}
	// 대쉬 중 중력 영향을 받지 않음
	if (true == m_isDash)
	{
		m_remainDashTime += DT;
		static float time  = 0.f;
		static int   index = 0;
		time += DT;

		if (time >= 0.02f)
		{
			CGameObject* pAfterImage =
				CMgrScript<CObjectManagerScript>::GetInst()->GetObjectFromPool(m_keyPrefabAfterImage);
			CSceneMgr::GetInst()->SpawnObject(pAfterImage, DEFAULT_LAYER);
			CAfterImageScript* pScript = static_cast<CAfterImageScript*>(pAfterImage->
				GetScript(static_cast<int>(SCRIPT_TYPE::AFTERIMAGESCRIPT)));
			pScript->ResetObject();
			pScript->SetNumber((++index));
			pScript->SetAlpha(1.f - index * 0.1f);
			time = 0.f;
		}
		if (DASH_TIME <= m_remainDashTime)
		{
			m_isDash         = false;
			m_remainDashTime = 0.f;
			index            = 0;
			m_pUnitScript->GetGravityScript()->SetGravity(true);
		}
	}

	Transform()->SetRelativePos(position + moveAmount);
}

void CPlayerScript::UpdateAction()
{
	if (KEY_TAP(KEY::V))
	{
		if (m_pInventoryUIScript->GetOwner()->IsActive())
		{
			m_pInventoryUIScript->GetOwner()->Deactivate();
			m_pPlayerInfoUIScript->HideWeaponUI(false);
		}
		else
		{
			m_pInventoryUIScript->GetOwner()->Activate();
			m_pPlayerInfoUIScript->HideWeaponUI(true);
			CResMgr::GetInst()->Load<CSound>(OPEN_INVENTORY, OPEN_INVENTORY)->Play(1, 1.f);
		}
	}
	if (KEY_TAP(KEY::TILED))
	{
		m_pInventoryUIScript->ChangeEquipSlot();
		m_pPlayerInfoUIScript->SwapWeaponSlot();
		m_pItemHolderScript->ChangeWeaponSlot();
		CResMgr::GetInst()->Load<CSound>(WEAPON_SWAP, WEAPON_SWAP)->Play(1, 1.f);
	}
	if (KEY_TAP(KEY::LBTN))
	{
		if (m_pInventoryUIScript->GetOwner()->IsActive()) { return; }

		const Vec3& playerPosition = Transform()->GetRelativePos();
		const Vec3& mousePosition = CMgrScript<CObjectManagerScript>::GetInst()->GetMouse()->Transform()->
			GetRelativePos();
		Vec3 directionVector = mousePosition - playerPosition;
		directionVector.Normalize();

		const float radian = atan2(directionVector.y, directionVector.x);

		m_pItemHolderScript->Attack(directionVector, Vec3{ 0.f, 0.f, radian });
	}
}

void CPlayerScript::FindAllChild()
{
	m_vecHands.resize(2);
	auto& vecChild = GetOwner()->GetChild();
	for (auto& pChild : vecChild)
	{
		if (L"DustEffect" == pChild->GetName())
		{
			m_pDustEffect = pChild;
		}
		else if (L"Left_Hand" == pChild->GetName())
		{
			m_vecHands[0] = pChild;
		}
		else if (L"Right_Hand" == pChild->GetName())
		{
			//항상 앞쪽에 위치해야 하므로.
			m_vecHands[1] = pChild;
		}
	}
	//최초 오른쪽을 바라보고 있으므로 그리고 무기는 오른손에만 착용하고, 회전해도 오른손이다.
	m_pWeaponHand = m_vecHands[static_cast<UINT>(UNIT_DIRECTION::RIGHT)];
	m_pItemHolderScript
		= static_cast<CItemHolderScript*>(m_pWeaponHand->GetScript(static_cast<UINT>(SCRIPT_TYPE::ITEMHOLDERSCRIPT)));
}

void CPlayerScript::ChangeCostume(COSTUME_TYPE type)
{
	if (type == m_costumeType) { return; }

	m_pUnitScript->ResetAnimationList();
	const std::wstring animationName = g_prefix[static_cast<UINT>(type)];
	const std::wstring idle = g_suffix[static_cast<UINT>(UNIT_STATE::IDLE)];
	const std::wstring move = g_suffix[static_cast<UINT>(UNIT_STATE::MOVE)];
	const std::wstring jump = g_suffix[static_cast<UINT>(UNIT_STATE::JUMP)];
	//const std::wstring die = g_suffix[static_cast<UINT>(UNIT_STATE::DEAD)];

	m_pUnitScript->SetAnimation(UNIT_STATE::IDLE, animationName + idle);
	m_pUnitScript->SetAnimation(UNIT_STATE::MOVE, animationName + move);
	m_pUnitScript->SetAnimation(UNIT_STATE::JUMP, animationName + jump);
	//m_pUnitScript->SetAnimation(UNIT_STATE::DEAD, animationName + die);
}

void CPlayerScript::SetState()
{
	switch (m_pUnitScript->GetUnitState())
	{
	case UNIT_STATE::IDLE:
	{
		if (KEY_PRESSED(KEY::D) || KEY_PRESSED(KEY::A))
		{
			m_pUnitScript->SetUnitState(UNIT_STATE::MOVE);
			m_pUnitScript->SetRepeat(true);
		}
		break;
	}
	case UNIT_STATE::MOVE:
	{
		if (KEY_PRESSED(KEY::D) == false && KEY_PRESSED(KEY::A) == false)
		{
			m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
			m_pUnitScript->SetRepeat(true);
		}

		break;
	}
	case UNIT_STATE::JUMP:
	{
		if (m_pUnitScript->IsGround() == true)
		{
			m_pUnitScript->SetUnitState(UNIT_STATE::IDLE);
		}
		break;
	}
	default:
		break;
	}
}

void CPlayerScript::SetDirection()
{
	if (m_pItemHolderScript->IsAttack())
	{
		return;
	}
	//마우스 위치에 따라 손을 회전 시킨다.
	const Vec3 mousePos = m_pMouseScript->Transform()->GetRelativePos();
	const Vec3 playerPos = Transform()->GetRelativePos();
	Vec3       direction = mousePos - playerPos;
	direction.Normalize();
	const float angleRadian = atan2(direction.y, direction.x);

	const Vec3& handPos = m_pWeaponHand->Transform()->GetRelativePos();
	const Vec3& attackRotation = m_pItemHolderScript->GetWeaponRotation();
	//어차피 회전을 해도 오른손에 아이템이 착용된 것은 동일하다.
	if (m_pMouseScript->IsLeft())
	{
		m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::LEFT);
		m_pWeaponHand->Transform()->SetRelativeRotation(attackRotation + Vec3{ 0.f, 0.f, XM_PI - angleRadian });

		//항상 플레이어 뒤에 위치해야하므로, 회전하면 z값 변경 필요
		m_pWeaponHand->Transform()->SetRelativePos(Vec3{ handPos.x, handPos.y, -5.f });
	}
	else
	{
		m_pUnitScript->SetUnitDirection(UNIT_DIRECTION::RIGHT);
		m_pWeaponHand->Transform()->SetRelativeRotation(attackRotation + Vec3{ 0.f, 0.f, angleRadian });
		m_pWeaponHand->Transform()->SetRelativePos(Vec3{ handPos.x, handPos.y, 5.f });
	}
}

void CPlayerScript::ShowDustEffect()
{
	if (m_showDustEffect
		&& m_pUnitScript->IsGround())
	{
		if (false == m_pDustEffect->IsActive())
		{
			m_pDustEffect->Activate();
		}
		m_pDustEffect->Animator2D()->Play(L"DustEffect", true);
	}
	else
	{
		if (m_pDustEffect->IsActive())
			m_pDustEffect->Deactivate();
	}
}
