#include "pch.h"
#include "CResMgr.h"

#include "CParticleUpdateShader.h"
#include "CTestShader.h"

#include "GameShaderInfo.h"

namespace
{
#pragma region MESH

	void CreatePointMesh(CResMgr* ptr)
	{
		CMesh* pMesh = nullptr;

		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;

		Vtx v;

		// ==========
		// Point Mesh	
		// ==========
		v.vPos   = Vec3(0.f, 0.f, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV    = Vec2(0.f, 0.f);
		vecVtx.push_back(v);
		vecIdx.push_back(0);

		pMesh = new CMesh;
		pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
		ptr->AddRes<CMesh>(L"PointMesh", pMesh, true);
	}

	void CreateRectMesh(CResMgr* ptr)
	{
		CMesh* pMesh = nullptr;

		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;

		Vtx v;

		// ========
		// RectMesh
		// 0 --- 1
		// |  \  |
		// 3 --- 2
		// ========
		v.vPos   = Vec3(-0.5f, 0.5f, 0.f);
		v.vColor = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vUV    = Vec2(0.f, 0.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(0.5f, 0.5f, 0.f);
		v.vColor = Vec4(0.2f, 1.f, 0.2f, 1.f);
		v.vUV    = Vec2(1.f, 0.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(0.5f, -0.5f, 0.f);
		v.vColor = Vec4(0.2f, 0.2f, 1.f, 1.f);
		v.vUV    = Vec2(1.f, 1.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(-0.5f, -0.5f, 0.f);
		v.vColor = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vUV    = Vec2(0.f, 1.f);
		vecVtx.push_back(v);

		vecIdx.push_back(0);
		vecIdx.push_back(2);
		vecIdx.push_back(3);
		vecIdx.push_back(0);
		vecIdx.push_back(1);
		vecIdx.push_back(2);

		pMesh = new CMesh;
		pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
		ptr->AddRes<CMesh>(L"RectMesh", pMesh, true);
	}

	void CreateRectMeshLineStrip(CResMgr* ptr)
	{
		CMesh* pMesh = nullptr;

		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;

		Vtx v;

		// ========
		// RectMesh
		// 0 --- 1
		// |  \  |
		// 3 --- 2
		// ========
		v.vPos   = Vec3(-0.5f, 0.5f, 0.f);
		v.vColor = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vUV    = Vec2(0.f, 0.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(0.5f, 0.5f, 0.f);
		v.vColor = Vec4(0.2f, 1.f, 0.2f, 1.f);
		v.vUV    = Vec2(1.f, 0.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(0.5f, -0.5f, 0.f);
		v.vColor = Vec4(0.2f, 0.2f, 1.f, 1.f);
		v.vUV    = Vec2(1.f, 1.f);
		vecVtx.push_back(v);

		v.vPos   = Vec3(-0.5f, -0.5f, 0.f);
		v.vColor = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vUV    = Vec2(0.f, 1.f);
		vecVtx.push_back(v);

		vecIdx.push_back(0);
		vecIdx.push_back(1);
		vecIdx.push_back(2);
		vecIdx.push_back(3);
		vecIdx.push_back(0);

		pMesh = new CMesh;
		pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
		ptr->AddRes<CMesh>(L"RectMesh_LineStrip", pMesh, true);
	}

	void CreateCircleMesh(CResMgr* ptr)
	{
		CMesh* pMesh = nullptr;

		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;

		Vtx v;

		// ==========
		// CircleMesh
		// ==========
		v.vPos   = Vec3(0.f, 0.f, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV    = Vec2(0.5f, 0.5f);
		vecVtx.push_back(v);

		UINT  iSliceCount = 40;
		float fRadius     = 0.5f;
		float fAngleStep  = XM_2PI / static_cast<float>(iSliceCount);

		for (UINT i = 0; i < iSliceCount + 1; ++i)
		{
			v.vPos = Vec3(fRadius * cosf(fAngleStep * static_cast<float>(i))
			            , fRadius * sinf(fAngleStep * static_cast<float>(i))
			            , 0.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vUV    = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
			vecVtx.push_back(v);
		}


		for (UINT i = 0; i < iSliceCount + 1; ++i)
		{
			vecIdx.push_back(0);
			vecIdx.push_back(i + 2);
			vecIdx.push_back(i + 1);
		}

		pMesh = new CMesh;
		pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
		ptr->AddRes<CMesh>(L"CircleMesh", pMesh, true);
	}

	void CreateCircleMeshLineStrip(CResMgr* ptr)
	{
		CMesh* pMesh = nullptr;

		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;

		Vtx v;

		// ==========
		// CircleMesh
		// ==========
		v.vPos   = Vec3(0.f, 0.f, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV    = Vec2(0.5f, 0.5f);
		vecVtx.push_back(v);

		UINT  iSliceCount = 40;
		float fRadius     = 0.5f;
		float fAngleStep  = XM_2PI / static_cast<float>(iSliceCount);

		for (UINT i = 0; i < iSliceCount + 1; ++i)
		{
			v.vPos = Vec3(fRadius * cosf(fAngleStep * static_cast<float>(i))
			            , fRadius * sinf(fAngleStep * static_cast<float>(i))
			            , 0.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vUV    = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
			vecVtx.push_back(v);
		}

		// CicleMesh_LineStrip
		for (UINT i = 1; i <= iSliceCount + 1; ++i)
		{
			vecIdx.push_back(i);
		}

		pMesh = new CMesh;
		pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
		ptr->AddRes<CMesh>(L"CircleMesh_LineStrip", pMesh, true);
	}
#pragma endregion

#pragma region SHADER

	void CreateSTD2D(CResMgr* ptr)
	{
		CGraphicsShader* pShader = nullptr;

		// Std2D Shader
		pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
		pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::DEFAULT);

		pShader->AddScalarParamInfo(L"Mask Limit", SCALAR_PARAM::FLOAT_0);

		pShader->AddScalarParamInfo(L"LeftTop", SCALAR_PARAM::VEC2_0);
		pShader->AddScalarParamInfo(L"Scale", SCALAR_PARAM::VEC2_1);
		pShader->AddScalarParamInfo(L"BackgroundSize", SCALAR_PARAM::VEC2_2);

		pShader->AddTexParamInfo(L"OutputTex 0", TEX_PARAM::TEX_0);
		pShader->AddTexParamInfo(L"OutputTex 1", TEX_PARAM::TEX_1);
		pShader->AddTexParamInfo(L"OutputTex 2", TEX_PARAM::TEX_2);

		ptr->AddRes<CGraphicsShader>(L"Std2DShader", pShader, true);
	}

	void CreateSTD2DAlphaBlend(CResMgr* ptr)
	{
		CGraphicsShader* pShader = nullptr;
		// Std2DAlphaBlend Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2DAlpha");
		pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2DAlpha");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		//pShader->SetDSType(DS_TYPE::NO_WRITE);

		pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);

		ptr->AddRes<CGraphicsShader>(L"Std2DAlphaBlendShader", pShader, true);
	}

	void CreatePaperBurn(CResMgr* ptr)
	{
		CGraphicsShader* pShader = nullptr;
		// PaperBurn Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"shader\\paperburn.fx", "VS_PaperBurn");
		pShader->CreatePixelShader(L"shader\\paperburn.fx", "PS_PaperBurn");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::DEFAULT);

		pShader->AddScalarParamInfo(L"Burn Strength", SCALAR_PARAM::FLOAT_0);
		pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);

		ptr->AddRes<CGraphicsShader>(L"PaperBurnShader", pShader, true);
	}

	void CreateTileMap(CResMgr* ptr)
	{
		CGraphicsShader* pShader = nullptr;
		// TileMap Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
		pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_NONE);

		pShader->AddTexParamInfo(L"TileMapAtlas", TEX_PARAM::TEX_0);

		ptr->AddRes<CGraphicsShader>(L"TileMapShader", pShader, true);
	}

	void CreateCollider2D(CResMgr* ptr)
	{
		CGraphicsShader* pShader{};
		// Collider2D Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Collider2D");
		pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Collider2D");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		ptr->AddRes<CGraphicsShader>(L"Collider2DShader", pShader, true);
	}

	void CreateParticleRender(CResMgr* ptr)
	{
		CGraphicsShader* pShader{};
		// Particle Render Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"Shader\\particlerender.fx", "VS_ParticleRender");
		pShader->CreateGeometryShader(L"Shader\\particlerender.fx", "GS_ParticleRender");
		pShader->CreatePixelShader(L"Shader\\particlerender.fx", "PS_ParticleRender");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pShader->SetDSType(DS_TYPE::NO_WRITE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetRSType(RS_TYPE::CULL_NONE);

		ptr->AddRes<CGraphicsShader>(L"ParticleRenderShader", pShader, true);
	}

	void CreatePostProcess(CResMgr* ptr)
	{
		CGraphicsShader* pShader{};
		// PostProcess Shader
		pShader = new CGraphicsShader;

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
		pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_PostProcess");
		pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_PostProcess");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		ptr->AddRes<CGraphicsShader>(L"PostProcessShader", pShader, true);
	}
#pragma endregion

#pragma region MATERIAL
	void CreateSTD2DMaterial(CResMgr* ptr)
	{
		// Std2DMtrl 생성
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"Std2DShader"));
		ptr->AddRes<CMaterial>(L"material\\Std2DMtrl.mtrl", pMtrl);
	}

	void CreateSTD2DAlphaBlendMaterial(CResMgr* ptr)
	{
		// Std2DAlphaBlend
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"Std2DAlphaBlendShader"));
		ptr->AddRes<CMaterial>(L"material\\Std2DAlphaBlendMtrl.mtrl", pMtrl);
	}

	void CreatePaperBurnMaterial(CResMgr* ptr)
	{
		// PaperBurnMtrl	
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"PaperBurnShader"));
		ptr->AddRes<CMaterial>(L"material\\PaperBurnMtrl.mtrl", pMtrl);
	}

	void CreateTileMapMaterial(CResMgr* ptr)
	{
		// TileMapMtrl
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"TileMapShader"));
		ptr->AddRes<CMaterial>(L"material\\TileMapMtrl.mtrl", pMtrl);
	}

	void CreateCollider2DMaterial(CResMgr* ptr)
	{
		// Collider2DMtrl 
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"Collider2DShader"));
		ptr->AddRes<CMaterial>(L"material\\Collider2DMtrl.mtrl", pMtrl);
	}

	void CreateParticleRenderMaterial(CResMgr* ptr)
	{
		// Particle Render Mtrl
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"ParticleRenderShader"));
		ptr->AddRes<CMaterial>(L"material\\ParticleRenderMtrl.mtrl", pMtrl);
	}

	void CreatePostProcessMaterial(CResMgr* ptr)
	{
		// PostProcess Mtrl
		CMaterial* pMtrl = new CMaterial;
		pMtrl->SetShader(ptr->FindRes<CGraphicsShader>(L"PostProcessShader"));
		pMtrl->SetTexParam(TEX_PARAM::TEX_0, ptr->FindRes<CTexture>(L"PostProcessTex"));
		ptr->AddRes<CMaterial>(L"material\\PostProcessMtrl.mtrl", pMtrl);
	}
#pragma endregion

#pragma region COMPUTE_SHADER

	void CreateTestCompute(CResMgr* ptr)
	{
		CComputeShader* pCS = new CTestShader{};
		pCS->CreateComputeShader(L"Shader\\testcs.fx", "CS_Test");
		ptr->AddRes<CComputeShader>(L"TestCS", pCS, true);
	}

	void CreateParticleUpdateCompute(CResMgr* ptr)
	{
		CComputeShader* pCS = new CParticleUpdateShader{};
		pCS->CreateComputeShader(L"Shader\\particle.fx", "CS_Particle");
		ptr->AddRes<CComputeShader>(L"ParticleUpdateShader", pCS, true);
	}

#pragma endregion

	constexpr UINT NOISE_TEXTURE_REGISTER_NUM_01 = 70;
	constexpr UINT NOISE_TEXTURE_REGISTER_NUM_02 = 71;
	constexpr UINT NOISE_TEXTURE_REGISTER_NUM_03 = 72;

#pragma region GAME_SHADER
	void CreatePlayerHPBarShader(CResMgr* ptr)
	{
		// Player HP bar Shader
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\playerHPBar.fx", "VS_PlayerHPBar");
		pShader->CreatePixelShader(L"shader\\playerHPBar.fx", "PS_PlayerHPBar");
		pShader->SetBSType(BS_TYPE::DEFAULT);
		pShader->SetDSType(DS_TYPE::LESS);
		pShader->SetRSType(RS_TYPE::CULL_BACK);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
		ptr->AddRes<CGraphicsShader>(PLAYER_HP_BAR_SHADER, pShader, true);
	}

	void CreateFadeEffectShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\fadeEffect.fx", "VS_FadeEffect");
		pShader->CreatePixelShader(L"shader\\fadeEffect.fx", "PS_FadeEffect");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::NO_TEST);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
		ptr->AddRes<CGraphicsShader>(FADE_EFFECT_SHADER, pShader, true);
	}

	void CreateMonsterHPBarShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\monsterHPBar.fx", "VS_MonsterHPBar");
		pShader->CreatePixelShader(L"shader\\monsterHPBar.fx", "PS_MonsterHPBar");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::DEFAULT);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		ptr->AddRes<CGraphicsShader>(MONSTER_HP_BAR_SHADER, pShader, true);
	}

	void CreateDamageUIShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\damageUI.fx", "VS_DamageUI");
		pShader->CreatePixelShader(L"shader\\damageUI.fx", "PS_DamageUI");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		ptr->AddRes<CGraphicsShader>(DAMAGE_UI_SHADER, pShader, true);
	}

	void CreateMonsterShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\monster.fx", "VS_Monster");
		pShader->CreatePixelShader(L"shader\\monster.fx", "PS_Monster");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::DEFAULT);
		pShader->SetDSType(DS_TYPE::LESS);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
		ptr->AddRes<CGraphicsShader>(MONSTER_SHADER, pShader, true);
	}

	void CreateAfterImageShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\afterImage.fx", "VS_AfterImage");
		pShader->CreatePixelShader(L"shader\\afterImage.fx", "PS_AfterImage");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::LESS);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		ptr->AddRes<CGraphicsShader>(AFTER_IMAGE_SHADER, pShader, true);
	}

	void CreatePlayerShader(CResMgr* ptr)
	{
		CGraphicsShader* pShader = new CGraphicsShader{};
		pShader->CreateVertexShader(L"shader\\player.fx", "VS_Player");
		pShader->CreatePixelShader(L"shader\\player.fx", "PS_Player");
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::LESS);
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
		ptr->AddRes<CGraphicsShader>(PLAYER_SHADER, pShader, true);
	}

	//COMPOUTE SHADER
	void CreateParticleDoorShaderCompute(CResMgr* ptr)
	{
		CComputeShader* pCS = new CParticleUpdateShader{};
		pCS->CreateComputeShader(L"Shader\\particleDoor.fx", "CS_ParticleDoor");
		ptr->AddRes<CComputeShader>(PARTICLE_DOOR_COMPUTE_SHADER, pCS, true);
	}

#pragma endregion

	void CreatePlayerHPBarMaterial(CResMgr* ptr)
	{
		// Player Hp bar Mtrl
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(PLAYER_HP_BAR_SHADER));
		ptr->AddRes<CMaterial>(PLAYER_HP_BAR_MATERIAL, pMaterial);
	}

	void CreateFadeEffectMaterial(CResMgr* ptr)
	{
		// fadeEffect Mtrl
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(FADE_EFFECT_SHADER));
		ptr->AddRes<CMaterial>(FADE_EFFECT_MATERIAL, pMaterial);
	}

	void CreateMonsterHPBarMaterial(CResMgr* ptr)
	{
		// fadeEffect Mtrl
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(MONSTER_HP_BAR_SHADER));
		ptr->AddRes<CMaterial>(MONSTER_HP_BAR_MATERIAL, pMaterial);
	}

	void CreateDamageUIMaterial(CResMgr* ptr)
	{
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(DAMAGE_UI_SHADER));
		ptr->AddRes<CMaterial>(DAMAGE_UI_MATERIAL, pMaterial);
	}

	void CreateMonsterMaterial(CResMgr* ptr)
	{
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(MONSTER_SHADER));
		ptr->AddRes<CMaterial>(MONSTER_MATERIAL, pMaterial);
	}

	void CreateAfterImageMaterial(CResMgr* ptr)
	{
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(AFTER_IMAGE_SHADER));
		ptr->AddRes<CMaterial>(AFTER_IMAGE_MATERIAL, pMaterial);
	}

	void CreatePlayerMaterial(CResMgr* ptr)
	{
		CMaterial* pMaterial = new CMaterial{};
		pMaterial->SetShader(ptr->FindRes<CGraphicsShader>(PLAYER_SHADER));
		ptr->AddRes<CMaterial>(PLAYER_MATERIAL, pMaterial);
	}
}

void CResMgr::init()
{
	InitSound();
	CreateEngineMesh();
	CreateEngineTexture();
	CreateEngineShader();
	CreateEngineMaterial();
	CreateEngineComputeShader();
}

void CResMgr::update()
{
	CSound::g_pFMOD->update();
}

void CResMgr::InitSound()
{
	System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CResMgr::CreateEngineMesh()
{
	CreatePointMesh(this);
	CreateRectMesh(this);
	CreateRectMeshLineStrip(this);
	CreateCircleMesh(this);
	CreateCircleMeshLineStrip(this);
}

void CResMgr::CreateEngineTexture()
{
	Ptr<CTexture> pNoise01    = Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png", true);
	Ptr<CTexture> pNoise02    = Load<CTexture>(L"texturenoise\\noise_02.png", L"texture\\noise\\noise_02.png", true);
	Ptr<CTexture> pNoiseCloud = Load<CTexture>(L"texture\\noise\\noise_cloud.jpg"
	                                         , L"texture\\noise\\noise_cloud.jpg"
	                                         , true);

	pNoise01->UpdateData(ALL, NOISE_TEXTURE_REGISTER_NUM_01);
	pNoise01->UpdateData_CS(NOISE_TEXTURE_REGISTER_NUM_01, true);

	pNoise02->UpdateData(ALL, NOISE_TEXTURE_REGISTER_NUM_02);
	pNoise02->UpdateData_CS(NOISE_TEXTURE_REGISTER_NUM_02, true);

	pNoiseCloud->UpdateData(ALL, NOISE_TEXTURE_REGISTER_NUM_03);
	pNoiseCloud->UpdateData_CS(NOISE_TEXTURE_REGISTER_NUM_03, true);

	g_global.vNoiseResolution = Vec2(pNoise01->Width(), pNoise01->Height());
}

void CResMgr::CreateEngineShader()
{
	MakeInputLayoutInfo();
	CreateSTD2D(this);
	CreateSTD2DAlphaBlend(this);
	CreatePaperBurn(this);
	CreateTileMap(this);
	CreateCollider2D(this);
	CreateParticleRender(this);
	CreatePostProcess(this);


	//GameSpecific
	CreatePlayerHPBarShader(this);
	CreateFadeEffectShader(this);

	CreateMonsterHPBarShader(this);
	CreateDamageUIShader(this);
	CreateMonsterShader(this);
	CreateAfterImageShader(this);
	CreatePlayerShader(this);
}

void CResMgr::CreateEngineMaterial()
{
	CreateSTD2DMaterial(this);
	CreateSTD2DAlphaBlendMaterial(this);
	CreatePaperBurnMaterial(this);
	CreateTileMapMaterial(this);
	CreateCollider2DMaterial(this);
	CreateParticleRenderMaterial(this);
	CreatePostProcessMaterial(this);

	//GameSpecific
	CreatePlayerHPBarMaterial(this);
	CreateFadeEffectMaterial(this);
	
	CreateMonsterHPBarMaterial(this);
	CreateDamageUIMaterial(this);
	CreateMonsterMaterial(this);
	CreateAfterImageMaterial(this);
	CreatePlayerMaterial(this);
}

void CResMgr::CreateEngineComputeShader()
{
	CComputeShader* pCS = nullptr;

	CreateTestCompute(this);
	CreateParticleUpdateCompute(this);

	//GameSpecific
	CreateParticleDoorShaderCompute(this);
}


void CResMgr::MakeInputLayoutInfo()
{
	// 정점 입력 구조 (InputLayout 생성)	
	UINT                     iOffset    = 0;
	D3D11_INPUT_ELEMENT_DESC tInputDesc = {};

	tInputDesc.SemanticName         = "POSITION";	// Semantic 이름
	tInputDesc.SemanticIndex        = 0;			// 중복 이름인 경우 인덱스로 구분
	tInputDesc.InputSlot            = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 12;

	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "COLOR";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 16;

	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "TEXCOORD";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 8;

	CGraphicsShader::AddInputLayout(tInputDesc);
}
