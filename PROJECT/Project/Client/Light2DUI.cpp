#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CLight2D.h>

Light2DUI::Light2DUI()
	:
	ComponentUI{"Light2D", COMPONENT_TYPE::LIGHT2D}
  , m_pLight2D{nullptr}
  , m_lightInfo{}
{
	SetSize(Vec2{0, 200});
}

Light2DUI::~Light2DUI() = default;

void Light2DUI::update()
{
	CGameObject* pTargetObj = GetTargetObject();

	if (nullptr == pTargetObj) { return; }

	m_pLight2D = pTargetObj->Light2D();

	m_lightInfo = m_pLight2D->GetLightInfo();

	ComponentUI::update();
}

void Light2DUI::render_update()
{
	ComponentUI::render_update();

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	ImGui::Text("LIGHT_TYPE");
	ImGui::SameLine(100);

	static const char* arrLightType[] = {"DIRECTIONAL", "POINT", "SPOT"};
	if (ImGui::BeginCombo("##LIGHT_TYPE_COMBO", arrLightType[m_lightInfo.iLightType]))
	{
		for (int i = 0; i < std::size(arrLightType); ++i)
		{
			if (ImGui::Selectable(arrLightType[i]))
			{
				m_pLight2D->SetLightType(static_cast<LIGHT_TYPE>(i));
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Text("DIFFUSE");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##DIFFUSE_COLOR", m_lightInfo.color.vDiff);
	m_pLight2D->SetDiffuse(m_lightInfo.color.vDiff);

	ImGui::Text("SPECULAR");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##SPECULAR_COLOR", m_lightInfo.color.vSpec);
	m_pLight2D->SetSpecular(m_lightInfo.color.vSpec);


	ImGui::Text("AMBIENT");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##AMBIENT_COLOR", m_lightInfo.color.vAmb);
	m_pLight2D->SetAmbient(m_lightInfo.color.vAmb);

	ImGui::Text("RANGE");
	ImGui::SameLine(100);
	ImGui::DragFloat("##LIGHT_RANGE", &m_lightInfo.fRange);
	m_pLight2D->SetRange(m_lightInfo.fRange);

	ImGui::Text("ANGLE");
	ImGui::SameLine(100);
	m_lightInfo.fAngle = XMConvertToDegrees(m_lightInfo.fAngle);
	ImGui::DragFloat("##LIGHT_ANGLE", &m_lightInfo.fAngle, 1.f, 0.f, 360.f);
	m_pLight2D->SetAngle(XMConvertToRadians(m_lightInfo.fAngle));

}
