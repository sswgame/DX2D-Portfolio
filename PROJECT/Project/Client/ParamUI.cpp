#include "pch.h"
#include "ParamUI.h"

#include "ImGui/imgui.h"
#include <Engine/CResMgr.h>
#include "CImGuiMgr.h"
#include "ImGui/imgui_stdlib.h"

int ParamUI::KeyCount = 0;

int ParamUI::Param_Int(const string& _strName, const int* _pInOut)
{
	// 파라미터 이름	
	ImGui::Text(_strName.c_str());
	ImGui::SameLine(100);

	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	int data = *_pInOut;
	ImGui::InputInt(szKey, &data);

	return data;
}

float ParamUI::Param_Float(const string& _strName, const float* _pInOut)
{
	// 파라미터 이름	
	ImGui::Text(_strName.c_str());
	ImGui::SameLine(100);

	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	float data = *_pInOut;
	ImGui::InputFloat(szKey, &data);

	return data;
}

Vec2 ParamUI::Param_Vec2(const string& _strName, const Vec2* _pInOut)
{
	// 파라미터 이름	
	ImGui::Text(_strName.c_str());
	ImGui::SameLine(100);

	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	float arrFloat2[2] = {_pInOut->x, _pInOut->y};

	ImGui::InputFloat2(szKey, arrFloat2);

	return Vec2(arrFloat2[0], arrFloat2[1]);
}

Vec4 ParamUI::Param_Vec4(const string& _strName, const Vec4* _pInOut)
{
	// 파라미터 이름
	ImGui::Text(_strName.c_str());
	ImGui::SameLine(100);

	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	Vec4 data = *_pInOut;

	ImGui::InputFloat4(szKey, data);

	return data;
}

void ParamUI::Param_Literal(const std::string& name, std::wstring* pInOut)
{
	// 파라미터 이름
	ImGui::Text(name.c_str());
	ImGui::SameLine(100);

	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	std::wstring       data = *pInOut;
	static std::string temp{};
	temp = ToString(data);
	ImGui::InputText(szKey, &temp);
	data    = ToWString(temp);
	*pInOut = data;
}

bool ParamUI::Param_Tex(const string& _strName, CTexture* _pCurTex, std::function<void(void* pData)> _pFunc)
{
	ImGui::Text(_strName.c_str());
	ImGui::SameLine(100);

	constexpr ImVec2 uv_min     = ImVec2(0.0f, 0.0f);                 // Top-left
	constexpr ImVec2 uv_max     = ImVec2(1.0f, 1.0f);                 // Lower-right
	constexpr ImVec4 tint_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	constexpr ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

	ImTextureID texID = nullptr;
	if (nullptr != _pCurTex)
	{
		texID = static_cast<ImTextureID>(_pCurTex->GetSRV().Get());
	}

	ImGui::Image(texID, ImVec2{150, 150}, uv_min, uv_max, tint_col, border_col);

	bool DragDropSuccess = false;
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
		{
			void* pData{};
			memcpy(pData, payload->Data, sizeof(void*));
			CRes* pRes = static_cast<CRes*>(pData);
			if (RES_TYPE::TEXTURE == pRes->GetResType())
			{
				static std::string key{};
				key = ToString(pRes->GetKey());

				CImGuiMgr::GetInst()->AddDelegate(tUIDelegate{_pFunc, &key});
				DragDropSuccess = true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (DragDropSuccess)
	{
		return true;
	}

	ImGui::SameLine();

	// List UI 활성화
	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	if (ImGui::Button(szKey, Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<std::wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		ListUI*                         pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("Texture List");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}
		pListUI->Activate();
		pListUI->SetDBCEvent(_pFunc);

		return true;
	}
	return false;
}

bool ParamUI::Param_Prefab(const std::string&               name
                         , const std::wstring&              prefabName
                         , std::function<void(void* pData)> pFunc)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();

	// List UI 활성화
	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	if (prefabName.empty())
	{
		ImGui::Text("NONE");
	}
	else
	{
		ImGui::Text(ToString(prefabName).c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button(szKey, Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<std::wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::PREFAB);
		ListUI*                         pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("PREFAB_LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}
		pListUI->Activate();
		pListUI->SetDBCEvent(pFunc);

		return true;
	}
	return false;
}

bool ParamUI::Param_Scene(const std::string& name
  , const std::wstring& prefabName
  , std::function<void(void* pData)> pFunc)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();

	// List UI 활성화
	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	if (prefabName.empty())
	{
		ImGui::Text("NONE");
	}
	else
	{
		ImGui::Text(ToString(prefabName).c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button(szKey, Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<std::wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::SCENEFILE);
		ListUI*                         pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("SCENE_LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}
		pListUI->Activate();
		pListUI->SetDBCEvent(pFunc);

		return true;
	}
	return false;
}

bool ParamUI::Param_Sound(const std::string&               name
                        , const std::wstring&              soundName
                        , std::function<void(void* pData)> pFunc)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();

	// List UI 활성화
	char szKey[255]{};
	sprintf_s(szKey, 255, "##Param%d", KeyCount++);

	if (soundName.empty())
	{
		ImGui::Text("NONE");
	}
	else
	{
		ImGui::Text(ToString(soundName).c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button(szKey, Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<std::wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::SOUND);
		ListUI*                         pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("SOUND_LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}
		pListUI->Activate();
		pListUI->SetDBCEvent(pFunc);

		return true;
	}
	return false;
}
