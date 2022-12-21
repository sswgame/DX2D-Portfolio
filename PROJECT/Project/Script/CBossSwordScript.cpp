#include "pch.h"
#include "CBossSwordScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr float SWORD_DELAY        = 1.f;
	constexpr float SWORD_SPEED        = 2300.f;
	constexpr float DESTROY_SWORD_TIME = 3.f;

	inline const wchar_t* SWORD = L"sound\\SWORD.wav";
}

CBossSwordScript::CBossSwordScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::BOSSSWORDSCRIPT), DEFINE_NAME(CBossSwordScript)}
  , m_accTime{0}
  , m_isMoving{false}
  , m_pPlayer{nullptr} {}

CBossSwordScript::~CBossSwordScript() = default;

void CBossSwordScript::start()
{
	m_pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();

	Animator2D()->Play(L"IDLE", true);

	Collider2D()->Deactivate();
}

void CBossSwordScript::update()
{
	m_accTime += DT;

	if (false == m_isMoving)
	{
		const Vec3& playerPos = m_pPlayer->Collider2D()->GetWorldPos();
		const Vec3  swordPos  = Collider2D()->GetWorldPos();

		Vec3 directionVector = playerPos - swordPos;
		directionVector.Normalize();

		const Vec3  baseVector   = {0.f, -1.f, 0.f};
		const float resultRadian = acosf(baseVector.Dot(directionVector));

		float rotationAngle = XMConvertToRadians(-90.f);
		rotationAngle += (playerPos.x - swordPos.x < 0.f) ? -resultRadian : resultRadian;

		Transform()->SetRelativeRotation(Vec3{0.f, 0.f, rotationAngle});

		m_direction.x = directionVector.x;
		m_direction.y = directionVector.y;

		if (m_accTime > SWORD_DELAY)
		{
			CResMgr::GetInst()->Load<CSound>(SWORD, SWORD)->Play(1, 1.f, true);
			m_isMoving = true;
			Animator2D()->Play(L"MOVE", true);
			if (false == Collider2D()->IsActive())
			{
				Collider2D()->Activate();
			}
		}
	}
	else
	{
		Vec3 position = Transform()->GetRelativePos();
		position.x += m_direction.x * DT * SWORD_SPEED;
		position.y += m_direction.y * DT * SWORD_SPEED;

		Transform()->SetRelativePos(position);
	}

	if (m_accTime > DESTROY_SWORD_TIME)
	{
		GetOwner()->Destroy();
	}
}
