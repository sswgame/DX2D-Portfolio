#include "pch.h"
#include "MaterialUI.h"


#include "ParamUI.h"
#include <Engine/CResMgr.h>

#include "ImGui/imgui_stdlib.h"

MaterialUI::MaterialUI()
	:
	ResInfoUI("Material", RES_TYPE::MATERIAL)
  , m_eSelectedTexParam(TEX_PARAM::END) {}

MaterialUI::~MaterialUI() {}

void MaterialUI::update()
{
	// To Do
	ResInfoUI::update();

	UI::update();
}

void MaterialUI::render_update()
{
	ResInfoUI::render_update();

	// To Do
	CMaterial* pMaterial = dynamic_cast<CMaterial*>(GetTargetRes());
	assert(pMaterial);

	// Material Key
	std::string key = ToString(pMaterial->GetKey());

	// Material Name
	ImGui::Text("MATERIAL");
	ImGui::SameLine(100);
	ImGui::InputText("##MaterialName", &key, ImGuiInputTextFlags_ReadOnly);

	// Shader Name
	CGraphicsShader* pShader = pMaterial->GetShader().Get();

	string shaderName{};
	if (nullptr != pShader)
		shaderName = ToString(pShader->GetKey());

	ImGui::Text("Shader");
	ImGui::SameLine(100);
	ImGui::InputText("##ShaderName", &shaderName, ImGuiInputTextFlags_ReadOnly);

	// Shader Parameter 확인
	if (nullptr == pShader) { return; }

	const vector<tScalarParamInfo>& vecScalarInfo = pShader->GetScalarParamInfo();
	for (const auto& [description, param] : vecScalarInfo)
	{
		std::string desc  = ToString(description);
		const void* pData = pMaterial->GetScalarParam(param);

		switch (param)
		{
		case SCALAR_PARAM::INT_0:
		case SCALAR_PARAM::INT_1:
		case SCALAR_PARAM::INT_2:
		case SCALAR_PARAM::INT_3:
			{
				int data = ParamUI::Param_Int(desc, static_cast<const int*>(pData));
				if (*static_cast<const int*>(pData) != data)
				{
					pMaterial->SetScalarParam(param, &data);
				}
			}
			break;
		case SCALAR_PARAM::FLOAT_0:
		case SCALAR_PARAM::FLOAT_1:
		case SCALAR_PARAM::FLOAT_2:
		case SCALAR_PARAM::FLOAT_3:
			{
				float data = ParamUI::Param_Float(desc, static_cast<const float*>(pData));
				if (*static_cast<const float*>(pData) != data)
				{
					pMaterial->SetScalarParam(param, &data);
				}
			}
			break;
		case SCALAR_PARAM::VEC2_0:
		case SCALAR_PARAM::VEC2_1:
		case SCALAR_PARAM::VEC2_2:
		case SCALAR_PARAM::VEC2_3:
			{
				Vec2 data = ParamUI::Param_Vec2(desc, static_cast<const Vec2*>(pData));
				if (*static_cast<const Vec2*>(pData) != data)
				{
					pMaterial->SetScalarParam(param, &data);
				}
			}
			break;
		case SCALAR_PARAM::VEC4_0:
		case SCALAR_PARAM::VEC4_1:
		case SCALAR_PARAM::VEC4_2:
		case SCALAR_PARAM::VEC4_3:
			{
				Vec4 data = ParamUI::Param_Vec4(desc, static_cast<const Vec4*>(pData));
				if (*static_cast<const Vec4*>(pData) != data)
				{
					pMaterial->SetScalarParam(param, &data);
				}
			}
			break;
		}
	}

	const vector<tTexParamInfo>& vecTexParamInfo = pShader->GetTexParamInfo();
	for (const auto& [description, texParam] : vecTexParamInfo)
	{
		string desc = ToString(description);

		switch (texParam)
		{
		case TEX_PARAM::TEX_0:
		case TEX_PARAM::TEX_1:
		case TEX_PARAM::TEX_2:
		case TEX_PARAM::TEX_3:
		case TEX_PARAM::TEX_4:
		case TEX_PARAM::TEX_5:
		case TEX_PARAM::TEX_CUBE_0:
		case TEX_PARAM::TEX_CUBE_1:
		case TEX_PARAM::TEX_ARR_0:
		case TEX_PARAM::TEX_ARR_1:
			if (ParamUI::Param_Tex(desc, pMaterial->GetTexParam(texParam).Get(), BIND_FN(MaterialUI::TextureSelected)))
			{
				m_eSelectedTexParam = texParam;
			}
			break;
		}
	}
}

// Delegate 용
void MaterialUI::TextureSelected(void* pName)
{
	const std::wstring key          = ToWString(static_cast<char*>(pName));
	CTexture*          pSelectedTex = CResMgr::GetInst()->FindRes<CTexture>(key).Get();
	CMaterial*         pMaterial    = dynamic_cast<CMaterial*>(GetTargetRes());
	assert(pMaterial);

	// 변경점이 있을 때만 세팅
	if (pMaterial->GetTexParam(m_eSelectedTexParam) != pSelectedTex)
	{
		pMaterial->SetTexParam(m_eSelectedTexParam, pSelectedTex);
	}
}
