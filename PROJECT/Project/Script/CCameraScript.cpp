#include "pch.h"
#include "CCameraScript.h"


#include <Engine/CCamera.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CMgrScript.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CTransform.h>

#include "CObjectManagerScript.h"
#include "NO_SCRIPT/Define.h"

namespace
{
	constexpr int SHAKE_AMOUNT = 4;
}

CCameraScript::CCameraScript()
	:
	CScript(static_cast<int>(SCRIPT_TYPE::CAMERASCRIPT), DEFINE_NAME(CCameraScript))
  , m_pPlayer{nullptr}
  , m_shakeTime{0} {}

CCameraScript::~CCameraScript() = default;

void CCameraScript::start()
{
	m_pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");
}

void CCameraScript::update()
{
	UpdatePosition();
	SetBound();
}

Vec2 CCameraScript::GetLeftTop()
{
	const float width = Camera()->GetWidth();
	const float height = width / Camera()->GetAspectRatio();
	const Vec3  position = Transform()->GetRelativePos();
	return Vec2{ position.x - width * 0.5f, position.y + height * 0.5f };
}

void CCameraScript::ShakeCamera(float time)
{
	m_shakeTime = time;
}

void CCameraScript::SetBound()
{
	const auto& vecRoots = CSceneMgr::GetInst()->GetCurScene()->GetLayer(TILE_LAYER)->GetRootObjects();
	if (vecRoots.size() == 0) { return; }
	static Vec2 resolution = CDevice::GetInst()->GetRenderResolution();
	const Vec3& tileMapScale = vecRoots.front()->Transform()->GetRelativeScale();
	Vec3 cameraPos = Transform()->GetRelativePos();

	const auto [left, top] = GetLeftTop();
	const auto [right, bottom] = Vec2{ cameraPos + Vec2{1.f, -1.f} *resolution * 0.5f };

	const float halfTileWidth = tileMapScale.x * 0.5f;
	const float halfTileHeight = tileMapScale.y * 0.5f;

	cameraPos.y = (top > halfTileHeight) ? cameraPos.y - (top - halfTileHeight) : cameraPos.y;
	cameraPos.x = (right > halfTileWidth) ? cameraPos.x - (right - halfTileWidth) : cameraPos.x;
	cameraPos.y = (bottom < -halfTileHeight) ? cameraPos.y + (-halfTileHeight - bottom) : cameraPos.y;
	cameraPos.x = (left < -halfTileWidth) ? cameraPos.x + (-halfTileWidth - left) : cameraPos.x;

	Transform()->SetRelativePos(cameraPos);
}

void CCameraScript::ShakeCamera(Vec3& pos)
{
	if (0.f < m_shakeTime)
	{
		m_shakeTime -= DT;

		SHAKE_DIR direction = static_cast<SHAKE_DIR>(GetRandomInRange(0, 3));

		switch (direction)
		{
		case SHAKE_DIR::UP:
			pos.y -= SHAKE_AMOUNT;
			break;
		case SHAKE_DIR::DOWN:
			pos.y += SHAKE_AMOUNT;
			break;
		case SHAKE_DIR::RIGHT:
			pos.x += SHAKE_AMOUNT;
			break;
		case SHAKE_DIR::LEFT:
			pos.x -= SHAKE_AMOUNT;
			break;
		case SHAKE_DIR::END:
			break;
		default:;
			break;
		}
		m_shakeTime = std::max(0.f, m_shakeTime);
	}
}

void CCameraScript::UpdatePosition()
{
	//맵이 카메라의 시야보다 큰 경우, 플레이어 위치를 기준으로 카메라를 이동시킨다.
	if (nullptr == m_pPlayer) { return; }

	m_pPlayer                 = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
	const Vec3 playerPosition = m_pPlayer->Transform()->GetRelativePos();
	Vec3       cameraPos      = Transform()->GetRelativePos();
	Vec3       newPos         = Vec3{playerPosition.x, playerPosition.y, cameraPos.z};

	ShakeCamera(newPos);
	//맵이 카메라의 시야보다 작은 경우, 해당 방향으로는 카메라를 이동시키지 않는다.
	const auto& vecRoots = CSceneMgr::GetInst()->GetCurScene()->GetLayer(TILE_LAYER)->GetRootObjects();
	if (vecRoots.size() == 0) { return; }

	const Vec3& tileMapScale = vecRoots.front()->Transform()->GetRelativeScale();
	if (tileMapScale.x <= GetOwner()->Camera()->GetWidth())
	{
		newPos.x = cameraPos.x;
	}
	if (tileMapScale.y <= GetOwner()->Camera()->GetHeight())
	{
		newPos.y = cameraPos.y;
	}
	
	Transform()->SetRelativePos(newPos);
}
