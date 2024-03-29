#include "pch.h"
#include "ComponentUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CComponent.h>

ComponentUI::ComponentUI(const string& _strName, COMPONENT_TYPE _eComType)
	:
	UI(_strName)
  , m_pTargetObject{nullptr}
  , m_eComType(_eComType)
  , m_bActive(false) {}

ComponentUI::~ComponentUI() {}

void ComponentUI::update()
{
	if (nullptr == m_pTargetObject)
		return;

	const CComponent* pComponent = m_pTargetObject->GetComponent(m_eComType);
	m_bActive                    = pComponent->IsActive();

	UI::update();
}

void ComponentUI::render_update()
{
	// 담당 Component 이름
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));
	ImGui::Button(ToString(m_eComType).c_str());
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// Component 활성화 체크
	CComponent* pComponent = m_pTargetObject->GetComponent(m_eComType);

	if (pComponent->GetType() != COMPONENT_TYPE::TRANSFORM)
	{
		bool IsActive = pComponent->IsActive();
		ImGui::SameLine();
		ImGui::Checkbox("##ComponentActive", &IsActive);

		if (pComponent->IsActive() != IsActive)
		{
			if (IsActive)
				pComponent->Activate();
			else
				pComponent->Deactivate();
		}
	}
	auto test = ImGui::GetWindowSize();
}
