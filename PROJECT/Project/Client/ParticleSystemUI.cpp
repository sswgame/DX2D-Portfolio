#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine/CParticleSystem.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

ParticleSystemUI::ParticleSystemUI()
	:
	ComponentUI{"ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM}
  , m_pParticleSystem{nullptr}
  , m_maxCount{5}
  , m_posInherit{false}
  , m_aliveCount{1}
  , m_arrMinMaxTime{{0.f}, {5.f}}
  , m_arrMinMaxSpeed{{10.f}, {100.f}}
  , m_range{10.f}
  , m_term{3.f}
{
	SetSize(Vec2{0, 300});
}

ParticleSystemUI::~ParticleSystemUI() {}

void ParticleSystemUI::update()
{
	CGameObject* pTargetObj = GetTargetObject();
	if (nullptr == pTargetObj)
		return;

	m_pParticleSystem = pTargetObj->ParticleSystem();

	m_maxCount   = m_pParticleSystem->GetMaxParticleCount();
	m_posInherit = m_pParticleSystem->PosInherit();
	m_aliveCount = m_pParticleSystem->GetAliveCount();

	auto [startLifeTime,endLifeTime] = m_pParticleSystem->GetMinMaxLifeTime();
	m_arrMinMaxTime[0]               = startLifeTime;
	m_arrMinMaxTime[1]               = endLifeTime;

	auto [starSpeed,endSpeed] = m_pParticleSystem->GetMinMaxSpeed();
	m_arrMinMaxSpeed[0]       = starSpeed;
	m_arrMinMaxSpeed[1]       = endSpeed;

	auto [startColor,endColor] = m_pParticleSystem->GetStartEndColor();
	m_arrStartEndColor[0]      = startColor;
	m_arrStartEndColor[1]      = endColor;

	auto [startScale,endScale] = m_pParticleSystem->GetStartEndScale();
	m_arrStartEndScale[0]      = startScale;
	m_arrStartEndScale[1]      = endScale;

	auto [leftRight,topBottom] = m_pParticleSystem->GetDirection();
	m_arrDirection[0]          = static_cast<int>(leftRight);
	m_arrDirection[1]          = static_cast<int>(topBottom);


	m_range = m_pParticleSystem->GetRange();
	m_term  = m_pParticleSystem->GetTerm();
}

void ParticleSystemUI::render_update()
{
	ComponentUI::render_update();

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	ImGui::Text("Shader");
	ImGui::SameLine(100);
	if (nullptr == m_pParticleSystem->GetComputeShader())
	{
		ImGui::Text("NONE");
	}
	else
	{
		ImGui::Text(ToString(m_pParticleSystem->GetComputeShader()->GetKey()).c_str());
	}
	ImGui::SameLine();

	if (ImGui::Button("##PARTICLE_SHADER", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const auto& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::COMPUTE_SHADER);
		ListUI*     pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("TEXTURE_LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(BIND_FN(ParticleSystemUI::ShaderSelect));
	}



	ImGui::Text("Pos Inherit");
	ImGui::SameLine(100);
	ImGui::Checkbox("##POS_INHERIT", &m_posInherit);
	m_pParticleSystem->SetPosInherit(m_posInherit);

	ImGui::Text("Max Count");
	ImGui::SameLine(100);
	ImGui::InputInt("##MAX_COUNT", &m_maxCount);
	m_pParticleSystem->SetMaxParticleCount(m_maxCount);

	ImGui::Text("Alive Count");
	ImGui::SameLine(100);
	ImGui::InputInt("##ALIVE_COUNT", &m_aliveCount);
	m_pParticleSystem->SetAliveCount(m_aliveCount);

	ImGui::Text("MIN/MAX Time");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##MIN_MAX_TIME", m_arrMinMaxTime);
	m_pParticleSystem->SetMinMaxLifeTime(m_arrMinMaxTime[0], m_arrMinMaxTime[1]);

	ImGui::Text("MIN/MAX Speed");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##MIN_MAX_SPEED", m_arrMinMaxSpeed);
	m_pParticleSystem->SetMinMaxSpeed(m_arrMinMaxSpeed[0], m_arrMinMaxSpeed[1]);

	ImGui::Text("Start Color");
	ImGui::SameLine(100);
	ImGui::ColorEdit4("##START_COLOR", m_arrStartEndColor[0]);
	ImGui::Text("End Color");
	ImGui::SameLine(100);
	ImGui::ColorEdit4("##END_COLOR", m_arrStartEndColor[1]);
	m_pParticleSystem->SetStartEndColor(m_arrStartEndColor[0], m_arrStartEndColor[1]);

	ImGui::Text("Start Scale");
	ImGui::SameLine(100);
	ImGui::InputFloat3("##START_SCALE", m_arrStartEndScale[0]);
	ImGui::Text("End Scale");
	ImGui::SameLine(100);
	ImGui::InputFloat3("##END_SCALE", m_arrStartEndScale[1]);
	m_pParticleSystem->SetStartEndScale(m_arrStartEndScale[0], m_arrStartEndScale[1]);

	ImGui::Text("Creation Range");
	ImGui::SameLine(100);
	ImGui::InputFloat("##CREATION_RANGE", &m_range);
	m_pParticleSystem->SetRange(m_range);

	ImGui::Text("Creation Term");
	ImGui::SameLine(100);
	ImGui::InputFloat("##CREATION_TERM", &m_term);
	m_pParticleSystem->SetTerm(m_term);

	ImGui::Text("DIRECTION");
	ImGui::SameLine((100));

	static std::map<std::string, Vec2> s_mapDirection
		= {
			{"FROM LEFT", Vec2{1.f, 0.f}}
		  , {"FROM RIGHT", Vec2{-1.f, 0.f}}
		  , {"FROM TOP", Vec2{0.f, 1.f}}
		  , {"FROM BOTTOM", Vec2{0.f, -1.f}}
		};
	static const char* S_DIRECTION[] = {"FROM LEFT", "FROM RIGHT", "FROM TOP", "FROM BOTTOM"};
	static Vec2        S_DIR[]       = {{1.f, 0.f}, {-1.f, 0.f}, {0.f, 1.f}, {0.f, -1.f}};
	int                index{};
	for (int i = 0; i < std::size(S_DIR); ++i)
	{
		if (S_DIR[i].x == m_arrDirection[0] && S_DIR[i].y == m_arrDirection[1])
		{
			index = i;
		}
	}
	if (ImGui::BeginCombo("##PARTICLE_DIRECTION_COMBO", S_DIRECTION[index]))
	{
		for (int i = 0; i < std::size(S_DIRECTION); ++i)
		{
			if (ImGui::Selectable(S_DIRECTION[i]))
			{
				m_pParticleSystem->SetDirection(s_mapDirection[S_DIRECTION[i]]);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Text("PARTICLE IMAGE");
	ImGui::SameLine();
	Ptr<CTexture> pTexture = m_pParticleSystem->GetMaterial()->GetTexParam(TEX_PARAM::TEX_0);
	if (nullptr != pTexture)
	{
		ImGui::Text(ToString(pTexture->GetKey()).c_str());
	}
	else
		ImGui::Text("NONE");
	ImGui::SameLine();
	if (ImGui::Button("##PARTICLE_TEXTURE_BUTTON", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const auto& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		ListUI*     pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("TEXTURE_LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(BIND_FN(ParticleSystemUI::TextureSelect));
	}
}

void ParticleSystemUI::TextureSelect(void* pTextureName)
{
	const std::wstring  key      = ToWString(static_cast<char*>(pTextureName));
	const Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(key);

	// 변경점이 있을 때만 세팅
	if (m_pParticleSystem->GetMaterial()->GetTexParam(TEX_PARAM::TEX_0) != pTexture)
	{
		m_pParticleSystem->GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, pTexture);
	}
}

void ParticleSystemUI::ShaderSelect(void* pShaderName)
{
	const std::wstring               key            = ToWString(static_cast<char*>(pShaderName));
	const Ptr<CParticleUpdateShader> pComputeShader = static_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<
		CComputeShader>(key).Get());

	// 변경점이 있을 때만 세팅
	if (m_pParticleSystem->GetComputeShader() != pComputeShader)
	{
		m_pParticleSystem->SetComputeShader(pComputeShader);
	}
}
