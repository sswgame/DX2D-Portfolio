#include "pch.h"
#include "TileMapUI.h"

#include <Engine/CTileMap.h>

#include "CImGuiMgr.h"
#include "TileMapToolUI.h"

TileMapUI::TileMapUI()
	:
	ComponentUI{"TileMap", COMPONENT_TYPE::TILEMAP}
  , m_pTileMapComponent{nullptr}
{}

TileMapUI::~TileMapUI() = default;

void TileMapUI::update()
{
	CGameObject* pTargetObject = GetTargetObject();
	if (nullptr == pTargetObject)
		return;

	m_pTileMapComponent = pTargetObject->TileMap();
	assert(m_pTileMapComponent);

	ComponentUI::update();

}

void TileMapUI::render_update()
{
	ComponentUI::render_update();

	Ptr<CTexture>     pTexture = m_pTileMapComponent->GetTexture();
	const std::string key      = (nullptr != pTexture) ? ToString(pTexture->GetKey()) : "NONE";
	ImGui::Text("TILE MAP TEXTURE : %s", key.c_str());

	ImGui::Text("TILE COUNT X : %d", m_pTileMapComponent->GetTileCountX());
	ImGui::Text("TILE COUNT Y : %d", m_pTileMapComponent->GetTileCountY());
	ImGui::Text("TILE SIZE  X : %d Y : %d"
	          , static_cast<UINT>(m_pTileMapComponent->GetTileSize().x)
	          , static_cast<UINT>(m_pTileMapComponent->GetTileSize().y));
	if (ImGui::Button("EDIT##TILE_MAP"))
	{
		TileMapToolUI* pTileMapToolUI = static_cast<TileMapToolUI*>(CImGuiMgr::GetInst()->FindUI("TileMapTool"));
		assert(pTileMapToolUI);
		if (pTileMapToolUI->IsActive()) return;

		pTileMapToolUI->SetTarget(m_pTileMapComponent->GetOwner());

		pTileMapToolUI->Activate();
	}
}
