#pragma once
#include "ComponentUI.h"

class ParticleSystemUI final
	: public ComponentUI
{
public:
	ParticleSystemUI();
	virtual ~ParticleSystemUI();

public:
	void update() override;
	void render_update() override;

private:
	void TextureSelect(void* pTextureName);
	void ShaderSelect(void* pShaderName);

	
private:
	CParticleSystem* m_pParticleSystem;

	int  m_maxCount;				// 파티클 최대 개수
	bool m_posInherit;				// 오브젝트 좌표에 영향
	int  m_aliveCount;				// 이번 프레임에 생성(활성화) 될 파티클 수

	float m_arrMinMaxTime[2];		// 파티클의 최소/최대 생명 시간
	float m_arrMinMaxSpeed[2];		// 파티클의 최소/최대 속력
	Vec4  m_arrStartEndColor[2];	// 파티클 초기/최종 색상
	Vec3  m_arrStartEndScale[2];    // 파티클 초기/최종 크기
	int   m_arrDirection[2];		// 파티클 방향

	float m_range;					// 파티클 생성 범위
	float m_term;					// 파티클 생성 간격


};


