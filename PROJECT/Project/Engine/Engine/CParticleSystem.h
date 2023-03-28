#pragma once
#include "CRenderComponent.h"

#include "CParticleUpdateShader.h"
#include "CStructuredBuffer.h"

#undef max
#undef min


class CParticleSystem : public CRenderComponent
{
private:
	Ptr<CParticleUpdateShader> m_pCS;           // ��ƼŬ ������Ʈ ���̴�

	CStructuredBuffer* m_pParticleBuffer;   // ��ƼŬ ����ȭ ����
	CStructuredBuffer* m_pDataBuffer;       // �߰� ������ ����(��ƼŬ AliveCount)
	UINT               m_maxCount;        // ��ƼŬ �ִ� ����

	int  m_isPosInherit;      // ������Ʈ ��ǥ�� ����
	UINT m_aliveCount;      // �̹� �����ӿ� ����(Ȱ��ȭ) �� ��ƼŬ ��

	float m_minLifeTime;     // ��ƼŬ�� �ּ� ���� �ð�
	float m_maxLifeTime;     // ��ƼŬ�� �ִ� ���� �ð�

	float m_startSpeed;      // ��ƼŬ�� �ּ� �ӷ�
	float m_endSpeed;        // ��ƼŬ�� �ִ� �ӷ�

	Vec4 m_startColor;      // ��ƼŬ �ʱ� ����
	Vec4 m_endColor;        // ��ƼŬ ���� ����

	Vec3 m_startScale;      // ��ƼŬ �ʱ� ũ��
	Vec3 m_endScale;        // ��ƼŬ ���� ũ��


	float m_creationRange;  // ��ƼŬ ���� ����
	float m_creationTerm;      // ��ƼŬ ���� ����
	float m_accTime;

	Vec2 m_direction;

public:
	void                       SetComputeShader(Ptr<CParticleUpdateShader> pCS) { m_pCS = pCS; }
	Ptr<CParticleUpdateShader> GetComputeShader() const { return m_pCS; }

	void                       SetMaxParticleCount(UINT count);
	UINT                       GetMaxParticleCount() const { return m_maxCount; }

	void SetPosInherit(bool enable);
	bool PosInherit() const { return m_isPosInherit; }

	void SetAliveCount(UINT count);
	UINT GetAliveCount() const { return m_aliveCount; }

	void SetMinMaxLifeTime(float min, float max);
	Vec2 GetMinMaxLifeTime() const { return Vec2{m_minLifeTime, m_maxLifeTime}; }

	void SetMinMaxSpeed(float min, float max);
	Vec2 GetMinMaxSpeed() const { return Vec2{m_startSpeed, m_endSpeed}; }

	void                  SetStartEndColor(Vec4 startColor, Vec4 endColor);
	std::pair<Vec4, Vec4> GetStartEndColor() const { return std::pair<Vec4, Vec4>{m_startColor, m_endColor}; }

	void                  SetStartEndScale(Vec3 startScale, Vec3 endScale);
	std::pair<Vec3, Vec3> GetStartEndScale() const { return std::pair<Vec3, Vec3>{m_startScale, m_endScale}; }

	void  SetRange(float range);
	float GetRange() const { return m_creationRange; }

	void  SetTerm(float term);
	float GetTerm() const { return m_creationTerm; }

	void SetDirection(Vec2 direction) { m_direction = direction; }
	Vec2 GetDirection() const { return m_direction; }
public:
	void finalupdate() override;
	void render() override;


public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CParticleSystem);
public:
	CParticleSystem();
	CParticleSystem(const CParticleSystem& _origin);
	virtual ~CParticleSystem();
};
