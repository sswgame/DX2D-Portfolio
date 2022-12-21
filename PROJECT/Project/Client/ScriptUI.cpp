#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CScript.h>
#include <Script/CScriptMgr.h>

#include "CImGuiMgr.h"
#include "ParamUI.h"

ScriptUI::ScriptUI()
	:
	UI("Script")
  , m_pTargetObject(nullptr)
  , m_pTargetScript(nullptr)
  , m_eComType(COMPONENT_TYPE::SCRIPT)
  , m_bActive(false)
  , m_pItemName{nullptr} { }

ScriptUI::~ScriptUI() {}

void ScriptUI::ItemSelected(void* pItemName) const
{
	*m_pItemName = ToWString(static_cast<char*>(pItemName));
}

void ScriptUI::render_update()
{
	// 담당 Script 이름
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.f, 0.8f, 0.8f)));

	const string scriptName = ToString(CScriptMgr::GetScriptName(m_pTargetScript));
	ImGui::Button(scriptName.c_str());

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	const vector<tScriptParamInfo>& vecParam = m_pTargetScript->GetScriptParam();

	for (auto parameterInfo : vecParam)
	{
		switch (parameterInfo.eType)
		{
		case SCRIPTPARAM_TYPE::INT:
			{
				const int data = ParamUI::Param_Int(parameterInfo.strParamName
				                                  , static_cast<const int*>(parameterInfo.pParam));
				*static_cast<int*>(parameterInfo.pParam) = data;
			}

			break;
		case SCRIPTPARAM_TYPE::FLOAT:
			{
				const float data = ParamUI::Param_Float(parameterInfo.strParamName
				                                      , static_cast<const float*>(parameterInfo.pParam));
				*static_cast<float*>(parameterInfo.pParam) = data;
			}

			break;
		case SCRIPTPARAM_TYPE::VEC2:
			{
				const Vec2 data = ParamUI::Param_Vec2(parameterInfo.strParamName
				                                    , static_cast<const Vec2*>(parameterInfo.pParam));
				*static_cast<Vec2*>(parameterInfo.pParam) = data;
			}
			break;
		case SCRIPTPARAM_TYPE::VEC4:
			{
				const Vec4 data = ParamUI::Param_Vec4(parameterInfo.strParamName
				                                    , static_cast<const Vec4*>(parameterInfo.pParam));
				*static_cast<Vec4*>(parameterInfo.pParam) = data;
			}
			break;
		case SCRIPTPARAM_TYPE::LITERAL:
			{
				ParamUI::Param_Literal(parameterInfo.strParamName, static_cast<std::wstring*>(parameterInfo.pParam));
			}
			break;
		case SCRIPTPARAM_TYPE::TEX:
			break;
		case SCRIPTPARAM_TYPE::PREFAB:
			{
				if (ParamUI::Param_Prefab(parameterInfo.strParamName
				                        , *static_cast<std::wstring*>(parameterInfo.pParam)
				                        , BIND_FN(ScriptUI::ItemSelected)))
				{
					m_pItemName = static_cast<std::wstring*>(parameterInfo.pParam);
				}
			}
			break;
			case SCRIPTPARAM_TYPE::SCENE:
			{
				if (ParamUI::Param_Scene(parameterInfo.strParamName
				                        , *static_cast<std::wstring*>(parameterInfo.pParam)
				                        , BIND_FN(ScriptUI::ItemSelected)))
				{
					m_pItemName = static_cast<std::wstring*>(parameterInfo.pParam);
				}
			}
			break;
		case SCRIPTPARAM_TYPE::SOUND:
			{
				if (ParamUI::Param_Sound(parameterInfo.strParamName
				                       , *static_cast<std::wstring*>(parameterInfo.pParam)
				                       , BIND_FN(ScriptUI::ItemSelected)))
				{
					m_pItemName = static_cast<std::wstring*>(parameterInfo.pParam);
				}
			}
			break;
		}
	}
}
