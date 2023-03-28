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

	int  m_maxCount;				// ��ƼŬ �ִ� ����
	bool m_posInherit;				// ������Ʈ ��ǥ�� ����
	int  m_aliveCount;				// �̹� �����ӿ� ����(Ȱ��ȭ) �� ��ƼŬ ��

	float m_arrMinMaxTime[2];		// ��ƼŬ�� �ּ�/�ִ� ���� �ð�
	float m_arrMinMaxSpeed[2];		// ��ƼŬ�� �ּ�/�ִ� �ӷ�
	Vec4  m_arrStartEndColor[2];	// ��ƼŬ �ʱ�/���� ����
	Vec3  m_arrStartEndScale[2];    // ��ƼŬ �ʱ�/���� ũ��
	int   m_arrDirection[2];		// ��ƼŬ ����

	float m_range;					// ��ƼŬ ���� ����
	float m_term;					// ��ƼŬ ���� ����


};


