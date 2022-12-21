#include "pch.h"
#include "UI.h"

namespace
{
	constexpr int DEFAULT_IMGUI_WINDOW_FLAG = 0;
}
UI::UI(const string& _strName)
	:
	m_name(_strName)
  , m_pParentUI(nullptr)
  , m_bOpen(true)
  , m_bModal(false)
  , m_windowFlags{DEFAULT_IMGUI_WINDOW_FLAG} {}

UI::~UI()
{
	Safe_Del_Vec(m_vecChildUI);
}

void UI::update()
{
	for (size_t i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->update();
	}
}

void UI::render()
{
	if (!(m_vPos.x == 0.f && m_vPos.y == 0.f) && m_bOpen)
	{
		ImGui::SetNextWindowPos(m_vPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2{m_vSize});
	}

	if (nullptr == m_pParentUI)
	{
		if (m_bOpen)
		{
			const string name = m_title + m_name;

			if (m_bModal)
			{
				// 모달은 가운데 고정
				const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				ImGui::OpenPopup(name.c_str());
				if (ImGui::BeginPopupModal(name.c_str(), &m_bOpen, m_windowFlags))
				{
					// 사이즈 재확인
					const ImVec2 size = ImGui::GetWindowSize();
					m_vSize.x         = size.x;
					m_vSize.y         = size.y;

					render_update();

					for (size_t i = 0; i < m_vecChildUI.size(); ++i)
					{
						m_vecChildUI[i]->render();
						ImGui::Separator();
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				ImGui::Begin(name.c_str(), &m_bOpen, m_windowFlags);

				// 사이즈 재확인
				const ImVec2 size = ImGui::GetWindowSize();
				m_vSize.x         = size.x;
				m_vSize.y         = size.y;

				render_update();

				for (size_t i = 0; i < m_vecChildUI.size(); ++i)
				{
					m_vecChildUI[i]->render();

					if (m_vecChildUI[i]->IsActive())
						ImGui::Separator();
				}

				ImGui::End();
			}
		}
	}

	else
	{
		if (m_bOpen)
		{
			ImGui::BeginChild(m_name.c_str(), m_vSize, false, m_windowFlags);

			render_update();

			for (size_t i = 0; i < m_vecChildUI.size(); ++i)
			{
				m_vecChildUI[i]->render();

				if (m_vecChildUI[i]->IsActive())
					ImGui::Separator();
			}

			ImGui::EndChild();

			CheckDragDrop();
		}
	}
}


Vec2::operator ImVec2() const
{
	return ImVec2(x, y);
}
