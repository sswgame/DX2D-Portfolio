#include "pch.h"
#include "CItemHolderScript.h"

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr int FIRST_ATTACK  = 1;
	constexpr int SECOND_ATTACK = 0;
}

CItemHolderScript::CItemHolderScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::ITEMHOLDERSCRIPT), DEFINE_NAME(CItemHolderScript)}
  , m_pCurrentWeapon{nullptr}
  , m_pCurrentWeaponScript{nullptr}
  , m_arrWeaponSlot{}
  , m_currentWeaponIndex{0}
  , m_pCameraScript{nullptr}
  , m_isOnAttack{false}
  , m_attackIndex{0}
  , m_finishAttack{false}
  , m_accTime{0}
  , m_attackDirection{0} {}

CItemHolderScript::~CItemHolderScript() = default;

void CItemHolderScript::start()
{
	CGameObject* pCamera = CMgrScript<CObjectManagerScript>::GetInst()->GetCamera();
	m_pCameraScript = static_cast<CCameraScript*>(pCamera->GetScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT)));
}

void CItemHolderScript::update()
{
	if (nullptr == m_pCurrentWeapon || nullptr == m_pCurrentWeaponScript) { return; }

	if (true == m_isOnAttack
	    && true == m_finishAttack
	    && true == m_pCurrentWeaponScript->CanAttack())
	{
		m_finishAttack = false;
	}

	if (false == m_finishAttack)
	{
		const float diff     = XM_PI / m_pCurrentWeaponScript->GetAttackSpeed() * DT;
		const Vec3  rotation = Transform()->GetRelativeRotation();

		if (FIRST_ATTACK == m_attackIndex)
		{
			Transform()->SetRelativeRotation(Vec3{0.f, 0.f, rotation.z - diff});
		}
		else
		{
			Transform()->SetRelativeRotation(Vec3{0.f, 0.f, rotation.z + diff});
		}

		m_accTime += DT;

		if (m_accTime >= m_pCurrentWeaponScript->GetAttackSpeed())
		{
			m_accTime = 0.f;

			m_finishAttack = true;
			m_isOnAttack   = false;
			m_pCurrentWeaponScript->SetCanAttack(false);
		}
	}

	if (false == m_pCurrentWeaponScript->CanAttack())
	{
		m_accTime += DT;
		if (m_accTime >= m_pCurrentWeaponScript->GetAttackDelay())
		{
			m_accTime = 0.f;
			m_pCurrentWeaponScript->SetCanAttack(true);
		}
	}


	if (KEY_TAP(KEY::Q) && m_pCurrentWeaponScript->GetSkillCoolTime() <= 0.f) {}

	if (m_pCurrentWeaponScript->GetSkillCoolTime() > 0.f)
	{
		/*m_SkillCoolTimer -= DT;
		m_SkillCoolTimer = std::max(0.f, m_SkillCoolTimer - DT);*/
	}
}

void CItemHolderScript::SetWeapon(CGameObject* pItem, int weaponSlotIndex)
{
	m_arrWeaponSlot[weaponSlotIndex] = pItem;

	if (nullptr != pItem)
	{
		if (m_currentWeaponIndex == weaponSlotIndex)
		{
			m_pCurrentWeapon       = m_arrWeaponSlot[weaponSlotIndex];
			m_pCurrentWeaponScript = static_cast<CWeaponScript*>(m_arrWeaponSlot[weaponSlotIndex]->
				GetScript(static_cast<UINT>(SCRIPT_TYPE::WEAPONSCRIPT)));
		}
	}
	else
	{
		m_arrWeaponSlot[weaponSlotIndex] = nullptr;
		if (m_currentWeaponIndex == weaponSlotIndex)
		{
			m_pCurrentWeapon       = nullptr;
			m_pCurrentWeaponScript = nullptr;
		}
	}
}


void CItemHolderScript::Attack(const Vec3& direction, const Vec3& rotation)
{
	if (nullptr == m_pCurrentWeaponScript || nullptr == m_pCurrentWeapon)
	{
		return;
	}

	if (m_pCurrentWeaponScript->CanAttack() && false == m_isOnAttack)
	{
		ShakeCamera();

		m_isOnAttack  = true;
		m_attackIndex = ++m_attackIndex % 2;
		if (false == m_pCurrentWeaponScript->IsEffectCreated())
		{
			const Vec3& weaponScale = m_pCurrentWeapon->Transform()->GetRelativeScale();
			const Vec3  test        = direction * weaponScale.y * 0.25f;
			m_pCurrentWeaponScript->InstanciateEffect(Transform()->GetWorldPos() + test, rotation);
		}
	}
}


void CItemHolderScript::ChangeWeaponSlot()
{
	if (nullptr != m_pCurrentWeapon)
	{
		if (m_pCurrentWeapon->IsActive())
			m_pCurrentWeapon->Deactivate();
	}

	m_currentWeaponIndex = ++m_currentWeaponIndex % 2;
	m_pCurrentWeapon     = m_arrWeaponSlot[m_currentWeaponIndex];

	if (nullptr != m_pCurrentWeapon)
	{
		m_pCurrentWeaponScript = static_cast<CWeaponScript*>(m_pCurrentWeapon->
			GetScript(static_cast<UINT>(SCRIPT_TYPE::WEAPONSCRIPT)));

		if (false == m_pCurrentWeapon->IsActive())
			m_pCurrentWeapon->Activate();
	}
}
